#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <string>
#include <array>

#include "glm/glm.hpp"

#include "AccelerationStructures/Improved/BVHBuilder.h"
#include "AccelerationStructures/Improved/ParallelBVHBuilder.h"

struct BenchAABB {
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

    BenchAABB() = default;
    BenchAABB(const glm::vec3& point, float pad)
    {
        glm::vec3 extent(pad);
        min = point - extent;
        max = point + extent;
    }

    BenchAABB(const BenchAABB& a, const BenchAABB& b)
    {
        min = glm::min(a.min, b.min);
        max = glm::max(a.max, b.max);
    }
};

struct BenchNode {
    BenchAABB box;
    BenchNode* left = nullptr;
    BenchNode* right = nullptr;

    explicit BenchNode(const BenchAABB& bounds) : box(bounds) {}
    BenchNode(BenchNode* l, BenchNode* r)
        : box(l && r ? BenchAABB(l->box, r->box) : (l ? l->box : r->box))
        , left(l)
        , right(r) {}
};

void DestroyTree(BenchNode* node)
{
    if (!node) return;
    DestroyTree(node->left);
    DestroyTree(node->right);
    delete node;
}

std::vector<glm::vec3> GeneratePositions(size_t count, float spread, std::mt19937& rng)
{
    std::uniform_real_distribution<float> dist(-spread, spread);
    std::vector<glm::vec3> points;
    points.reserve(count);
    for (size_t i = 0; i < count; ++i)
        points.emplace_back(dist(rng), dist(rng), dist(rng));
    return points;
}

struct EdgePayload { glm::vec3 a; glm::vec3 b; };

std::vector<EdgePayload> GenerateEdges(size_t count, std::mt19937& rng, float spread)
{
    std::uniform_real_distribution<float> dist(-spread, spread);
    std::vector<EdgePayload> edges;
    edges.reserve(count);
    for (size_t i = 0; i < count; ++i)
        edges.push_back({ glm::vec3(dist(rng), dist(rng), dist(rng)),
            glm::vec3(dist(rng), dist(rng), dist(rng)) });
    return edges;
}

struct FacePayload { std::array<glm::vec3, 3> verts; };

std::vector<FacePayload> GenerateFaces(size_t count, std::mt19937& rng, float spread)
{
    std::uniform_real_distribution<float> dist(-spread, spread);
    std::vector<FacePayload> faces;
    faces.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        FacePayload face;
        for (auto& v : face.verts)
            v = glm::vec3(dist(rng), dist(rng), dist(rng));
        faces.push_back(face);
    }
    return faces;
}

std::vector<glm::vec2> GenerateUVs(size_t count, std::mt19937& rng, float spread)
{
    std::uniform_real_distribution<float> dist(-spread, spread);
    std::vector<glm::vec2> uvs;
    uvs.reserve(count);
    for (size_t i = 0; i < count; ++i)
        uvs.emplace_back(dist(rng), dist(rng));
    return uvs;
}

BenchAABB MakePointBox(const glm::vec3& point, float pad = 0.05f)
{
    return BenchAABB(point, pad);
}

BenchAABB MakeEdgeBox(const glm::vec3& a, const glm::vec3& b)
{
    return BenchAABB(BenchAABB(a, 0.05f), BenchAABB(b, 0.05f));
}

BenchAABB MakeFaceBox(const std::array<glm::vec3, 3>& verts)
{
    BenchAABB box(verts[0], 0.0f);
    box = BenchAABB(box, BenchAABB(verts[1], 0.0f));
    box = BenchAABB(box, BenchAABB(verts[2], 0.0f));
    return box;
}

BenchAABB MakeUVBox(const glm::vec2& uv, float pad = 0.005f)
{
    return BenchAABB(glm::vec3(uv, 0.0f), pad);
}

template <typename Payloads, typename LeafFactory>
BenchNode* BuildSerial(const Payloads& payloads, LeafFactory&& leafFactory)
{
    return BVHBuilder::BuildBVH(payloads, std::forward<LeafFactory>(leafFactory));
}

template <typename Payloads, typename LeafFactory>
BenchNode* BuildParallel(const Payloads& payloads, LeafFactory&& leafFactory)
{
    return ParallelBVHBuilder::BuildBVH(payloads, std::forward<LeafFactory>(leafFactory));
}

template <typename Payloads, typename LeafFactory>
double MeasureBuild(bool parallel, const Payloads& payloads, LeafFactory&& leafFactory)
{
    auto start = std::chrono::steady_clock::now();
    BenchNode* root = parallel
        ? BuildParallel(payloads, std::forward<LeafFactory>(leafFactory))
        : BuildSerial(payloads, std::forward<LeafFactory>(leafFactory));
    auto end = std::chrono::steady_clock::now();
    DestroyTree(root);
    return std::chrono::duration<double, std::milli>(end - start).count();
}

template <typename Payloads, typename LeafFactory>
void RunScenario(const std::string& label, const Payloads& payloads, LeafFactory&& leafFactory)
{
    double serialMs = MeasureBuild(false, payloads, leafFactory);
    double parallelMs = MeasureBuild(true, payloads, leafFactory);

    std::cout << label << "\n";
    std::cout << "  Serial   : " << serialMs << " ms\n";
    std::cout << "  Parallel : " << parallelMs << " ms\n";
}

int main()
{
    omp_set_num_threads(16);
#pragma omp parallel
    {
        // Only one thread prints the total number of threads
#pragma omp parallel
        {
            int n = omp_get_num_threads();
            std::cout << "Number of threads in parallel region: " << n << "\n";
        }
    }
    std::mt19937 rng(42);
    const size_t objectCounts[] = { 10'000, 400'000 };
    const size_t vertexCounts[] = { 50'000, 1'000'000 };
    const size_t edgeCounts[] = { 25'000, 1'000'000 };
    const size_t faceCounts[] = { 12'000, 1'000'000 };
    const size_t uvCounts[] = { 50'000, 200'000 };

    //for (size_t count : objectCounts)
    //{
    //    auto positions = GeneratePositions(count, 1500.0f, rng);
    //    RunScenario("Objects: " + std::to_string(count), positions,
    //        [](const glm::vec3& p)
    //        {
    //            return new BenchNode(MakePointBox(p, 2.0f));
    //        });
    //}

    for (size_t count : vertexCounts)
    {
        auto positions = GeneratePositions(count, 2000.0f, rng);
        RunScenario("Vertices: " , positions,
            [](const glm::vec3& p)
            {
                return new BenchNode(MakePointBox(p, 0.05f));
            });
    }

 /*   for (size_t count : edgeCounts)
    {
        auto endpoints = GenerateEdges(count, rng, 1800.0f);
        RunScenario("Edges: " + std::to_string(count), endpoints,
            [](const EdgePayload& e)
            {
                return new BenchNode(MakeEdgeBox(e.a, e.b));
            });
    }

    for (size_t count : faceCounts)
    {
        auto faces = GenerateFaces(count, rng, 1200.0f);
        RunScenario("Faces: " + std::to_string(count), faces,
            [](const FacePayload& face)
            {
                return new BenchNode(MakeFaceBox(face.verts));
            });
    }

    for (size_t count : uvCounts)
    {
        auto uvs = GenerateUVs(count, rng, 2.0f);
        RunScenario("UVs: " + std::to_string(count), uvs,
            [](const glm::vec2& uv)
            {
                return new BenchNode(MakeUVBox(uv));
            });
    }*/

    return 0;
}
