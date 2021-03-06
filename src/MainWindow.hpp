#pragma once

#include "Common.hpp"

#include "VolumeData.hpp"
#include "RenderViewport.hpp"
#include "ControlPanel.hpp"


class MainWindow: public QMainWindow
{
	Q_OBJECT
	public:
		RenderViewport renderViewport;
		ControlPanel controlPanel;
		
		QMenu* fileMenu;
		QAction* saveAction;
		QAction* loadAction;
		QMenu* importAction;
		QMenu* importSequenceAction;
		
		MainWindow();
		void ExpandToFitScreen();
		
		
	public slots:
		void Save();
		void Load();
};