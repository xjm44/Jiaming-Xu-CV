#pragma once
#include <string>
#include <QListWidget>
#include <QtWidgets/QMainWindow>

enum class log_type {
	Info,
	Warning,
	Error
};

// for the console in the mainwindow
class logger {


public:

	logger();
	~logger();

	void set_log(log_type type , const std::string& msg, QListWidget *widget);

};