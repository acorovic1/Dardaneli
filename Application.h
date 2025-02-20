#pragma once


class Application {


	static Application* instance;

	Application() {};

public:

	static Application* getInstance();

	Application(const Application& copy) = delete;
	void operator=(const Application& copy) = delete;






};