#include "ControlPanel.hpp"


ControlPanel::ControlPanel(): QDockWidget("Control Panel")
{
	setMinimumWidth(300);
	
	scrollAreaBase = new QScrollArea();
	widgetBase = new QWidget();
	layoutBase = new QVBoxLayout();
	
	widgetBase->setLayout(layoutBase);
	scrollAreaBase->setWidget(widgetBase);
	scrollAreaBase->setWidgetResizable(true);
	setWidget(scrollAreaBase);
	
	layoutBase->setAlignment(Qt::AlignTop);
		
	//Base settings
	QHBoxLayout* modeLayout = new QHBoxLayout();
	layoutBase->addLayout(modeLayout); 
	button2D = new QPushButton("2D"); 
	button2D->setCheckable(true);
	button2D->setChecked(true);
	button2D->setEnabled(false);
	modeLayout->addWidget(button2D);
	button3D = new QPushButton("3D");
	button3D->setCheckable(true);
	modeLayout->addWidget(button3D);
	
	 
	//2D Settings
	groupBox2D = new QGroupBox("2D Settings");
	layoutBase->addWidget(groupBox2D);
	layoutGroup2D = new QVBoxLayout();
	layoutGroup2D->setMargin(2);
	groupBox2D->setLayout(layoutGroup2D);
	
	layoutGroup2D->addWidget(new QLabel("Brightness Contrast Threshold"));
	scalarChooserBrightness = new ScalarChooser("Brightness", -1000, 1000.0, 0.0, 0.1, false);
	layoutGroup2D->addWidget(scalarChooserBrightness);
	scalarChooserContrast = new ScalarChooser("Contrast", -1000, 1000.0, 0.0, 0.1, false);
	layoutGroup2D->addWidget(scalarChooserContrast);
	scalarChooserThreshold = new ScalarChooser("Threshold", -1000, 1000.0, 0.0, 0.001, false);
	layoutGroup2D->addWidget(scalarChooserThreshold);
	buttonBrightnessContrastApply = new QPushButton("Apply");
	layoutGroup2D->addWidget(buttonBrightnessContrastApply); 
	
	//3D Settings	
	groupBox3D = new QGroupBox("3D Settings");
	layoutBase->addWidget(groupBox3D);
	layoutGroup3D = new QVBoxLayout();
	layoutGroup3D->setMargin(2);
	groupBox3D->setLayout(layoutGroup3D);
	groupBox3D->setVisible(false); 
	
	checkAxisVisible = new QCheckBox("Axis Visible");
	layoutGroup3D->addWidget(checkAxisVisible);
	
	comboRenderer = new QComboBox();
	comboRenderer->addItem("Slice");
	comboRenderer->addItem("Ray");
	comboRenderer->addItem("Photon");
	layoutGroup3D->addWidget(comboRenderer);
	
	chooserPosition = new Vector3Chooser("position");		
	chooserRotation = new Vector3Chooser("rotation");
	chooserScale = new Vector3Chooser("scale", 1, 1, 1);
	//layoutGroup->addWidget(chooserPosition); 
	//layoutGroup->addWidget(chooserRotation); 
	layoutGroup3D->addWidget(chooserScale);
	
	buttonSampleMapping = new QPushButton("Sample Mapping"); 
	layoutGroup3D->addWidget(buttonSampleMapping);
	
	sampleMapping = new SampleMappingEditor;
	
	connect(buttonSampleMapping, &QPushButton::clicked, [this]()
	{
		sampleMapping->show();
	});
	
	chooserGradientThreshold = new ScalarChooser("Gradient Threshold", 0, 0.25, 0.05, 0.05);
	layoutGroup3D->addWidget(chooserGradientThreshold);
	
	checkBackFaceCulling = new QCheckBox(""); 
	layoutGroup3D->addWidget(new QLabel("Back Face Culling"));
	layoutGroup3D->addWidget(checkBackFaceCulling);
	checkBackFaceCulling->setCheckState(Qt::Checked);
	
	
	//mode selection
	connect(button3D, &QPushButton::clicked, [&, this](bool check){
		if(check == true)
		{
			button3D->setEnabled(false);
			button2D->setChecked(false);
			button2D->setEnabled(true);
			
			emit Enabled3DMode(); 
			std::cout << "button3D:clicked" << std::endl; 
		}
	}); 
	connect(button2D, &QPushButton::clicked, [&, this](bool check){
		if(check == true)
		{
			button2D->setEnabled(false);
			button3D->setChecked(false);
			button3D->setEnabled(true);
			
			emit Enabled2DMode(); 
			std::cout << "button2D:clicked" << std::endl; 
		}
	});
	
	connect(this, &ControlPanel::Enabled2DMode, [&, this](){
		groupBox2D->setVisible(true); 
		groupBox3D->setVisible(false); 
	});
	connect(this, &ControlPanel::Enabled3DMode, [&, this](){
		groupBox2D->setVisible(false);
		groupBox3D->setVisible(true); 		
	});
}
