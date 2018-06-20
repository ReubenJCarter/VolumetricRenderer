#pragma once


#include "Common.hpp"
#include "HelperWidgets.hpp"
#include "SampleMappingEditor.hpp"

class ControlPanel: public QDockWidget
{
	Q_OBJECT
	public:
		QScrollArea* scrollAreaBase;
		QWidget* widgetBase; 
		QVBoxLayout* layoutBase;
		
		//base setings
		QPushButton* button3D;
		QPushButton* button2D; 
		
		//2d settings
		QGroupBox* groupBox2D; 
		QVBoxLayout* layoutGroup2D;
		
		//3d settings
		QGroupBox* groupBox3D; 
		QVBoxLayout* layoutGroup3D;
		QCheckBox* checkAxisVisible;
		QComboBox* comboRenderer; 
		Vector3Chooser* chooserPosition;		
		Vector3Chooser* chooserRotation;
		Vector3Chooser* chooserScale;
		QPushButton* buttonSampleMapping; 
		SampleMappingEditor* sampleMapping;
		ScalarChooser* chooserGradientThreshold;
		QCheckBox* checkBackFaceCulling; 
	
		ControlPanel();
		
	signals:
		void Enabled2DMode();  
		void Enabled3DMode();  
};