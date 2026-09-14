#include "../../include/process/setting.h"
#include "../../include/process/processGui.h"

settingParam::settingParam(gui *parent)
	: QDialog(parent),window_(parent)
{
	//setWindowFlags(flags | Qt::Tool);
	this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	this->setFixedHeight(360);
	this->setFixedWidth(280);
	setupUi(this);


	init();

}

settingParam::~settingParam()
{
}
void settingParam::closeEvent(QCloseEvent* event) {
	std::cout << "params closed\n";
	emit(param_close());
}

void settingParam::_doubleSpinBox_lidarMaxXToFilter() {
	doubleSpinBox_lidarMinXToFilter->setMaximum(doubleSpinBox_lidarMaxXToFilter->value() - 0.01);
}

void settingParam::_checkBox_loopClosure(int b) {
	if (b) {
		spinBox_intervalFrames->setEnabled(true);
	}
	else {
		spinBox_intervalFrames->setDisabled(true);
	}
}

void settingParam::_btnClicked() {
	
	setParams();
	this->close();
	//std::cout << "from setting "  << std::endl;
	//std::cout << "core: " << std::to_string(no_of_cores) << std::endl;
	//std::cout << "loopClosure: " << std::to_string(loopClosure) << std::endl;
	//std::cout << "loopClosureIntervalFrames: " << std::to_string(loopClosureIntervalFrames) << std::endl;
	//std::cout << "narrowSpace: " << std::to_string(narrowSpace) << std::endl;
	//std::cout << "minIntensityFilter: " << std::to_string(minIntensityFilter) << std::endl;
	//std::cout << "lidarMaxRange: " << std::to_string(lidarMaxRange) << std::endl;
	//std::cout << "lidarMinRange: " << std::to_string(lidarMinRange) << std::endl;
	//std::cout << "lidarMaxXToFilter: " << std::to_string(lidarMaxXToFilter) << std::endl;
	//std::cout << "lidarMinXToFilter: " << std::to_string(lidarMinXToFilter) << std::endl;
	//std::cout << "translationThreshold: " << std::to_string(translationThreshold) << std::endl;
	//std::cout << "icpMaximumIterations: " << std::to_string(icpMaximumIterations) << std::endl;
	//std::cout << "------------------------" << std::endl;

}

void settingParam::init()
{
	// pre set parameters
	doubleSpinBox_lidarMinXToFilter->setMaximum(doubleSpinBox_lidarMaxXToFilter->value() - 0.01);
	spinBox_intervalFrames->setDisabled(true);


	connect(doubleSpinBox_lidarMaxXToFilter, SIGNAL(valueChanged(double)), this, SLOT(_doubleSpinBox_lidarMaxXToFilter()), Qt::UniqueConnection);
	connect(checkBox_loopClosure, SIGNAL(stateChanged(int)), this, SLOT(_checkBox_loopClosure(int)), Qt::UniqueConnection);

	connect(pushButton, SIGNAL(clicked()), this, SLOT(_btnClicked()), Qt::UniqueConnection);

	
	
}

void settingParam::setParams() {
	int no_of_cores = spinBox_noOfCores->value();
	bool loopClosure = checkBox_loopClosure->isChecked();
	int loopClosureIntervalFrames = spinBox_intervalFrames->value();
	bool narrowSpace = checkBox_narrowSpace->isChecked();
	float minIntensityFilter = doubleSpinBox_intensityFilter->value();
	float lidarMaxRange = doubleSpinBox_lidarMaxRange->value();
	float lidarMinRange = doubleSpinBox_lidarMinRange->value();
	float lidarMaxXToFilter = doubleSpinBox_lidarMaxXToFilter->value();
	float lidarMinXToFilter = doubleSpinBox_lidarMinXToFilter->value();
	float translationThreshold = doubleSpinBox_translationThreshold->value();
	int icpMaximumIterations = spinBox_icpMaximumIterations->value();
	window_->setParams_fromSetting(
		no_of_cores,
		loopClosure,
		loopClosureIntervalFrames,
		narrowSpace,
		minIntensityFilter,
		lidarMaxRange,
		lidarMinRange,
		lidarMaxXToFilter,
		lidarMinXToFilter,
		translationThreshold,
		icpMaximumIterations
	);
}