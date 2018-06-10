#include "MainWindow.hpp"


MainWindow::MainWindow()
{
	setWindowTitle("RenderViewport");
	
	//Set the Top bar 
	fileMenu = menuBar()->addMenu("File");
	saveAction = fileMenu->addAction("Save");
	loadAction = fileMenu->addAction("Load");
	importAction = fileMenu->addMenu("Import");
	importSequenceAction = fileMenu->addMenu("Import Sequence");
	
	QAction* tiffAction = importAction->addAction("tiff");
	QAction* imageAction = importAction->addAction("image");
	QAction* nrrdAction = importAction->addAction("nrrd");
	QAction* dcmSqeuenceAction = importSequenceAction->addAction("dcm");
	QAction* tiffSequenceAction = importSequenceAction->addAction("tiff");
	QAction* imageSequenceAction = importSequenceAction->addAction("image");
	
	QObject::connect(saveAction, SIGNAL(triggered()), this, SLOT(Save()));
	QObject::connect(loadAction, SIGNAL(triggered()), this, SLOT(Load()));
	
	
	QObject::connect(tiffAction, &QAction::triggered, [this]()
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("types of File(*)"));
		renderViewport.ImportTIFFFile(fileName, &(controlPanel.imageSettings->sampleMapping->histogram->data));
		std::cout << "Import tiff" << std::endl;
		
	});
	
	QObject::connect(nrrdAction, &QAction::triggered, [this]()
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("types of File(*)"));
		renderViewport.ImportNRRDFile(fileName, &(controlPanel.imageSettings->sampleMapping->histogram->data));
		std::cout << "Import image" << std::endl;
		
	});
	
	QObject::connect(imageAction, &QAction::triggered, [this]()
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("types of File(*)"));
		renderViewport.ImportImageFile(fileName, &(controlPanel.imageSettings->sampleMapping->histogram->data));
		std::cout << "Import image" << std::endl;
		
	});
	
	
	QObject::connect(dcmSqeuenceAction, &QAction::triggered, [this]()
	{
		QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), "", tr("types of Files(*)"));
		renderViewport.ImportDicomFileSequence(fileNames);
		std::cout << "Import Sequence" << std::endl;
	});
	QObject::connect(tiffSequenceAction, &QAction::triggered, [this]()
	{
		QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), "", tr("types of Files(*)"));
		renderViewport.ImportTIFFFileSequence(fileNames, &(controlPanel.imageSettings->sampleMapping->histogram->data));
		//renderViewport.ImportTIFFFileSequence(fileNames, NULL);
		std::cout << "Import Sequence" << std::endl;
		
	});
	QObject::connect(imageSequenceAction, &QAction::triggered, [this]()
	{
		QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image"), "", tr("types of Files(*)"));
		renderViewport.ImportImageFileSequence(fileNames, &(controlPanel.imageSettings->sampleMapping->histogram->data));
		//renderViewport.ImportTIFFFileSequence(fileNames, NULL);
		std::cout << "Import Sequence" << std::endl;
		
	});
	
	QObject::connect(controlPanel.checkAxisVisible, &QCheckBox::stateChanged, [this](int state)
	{
		if(state == Qt::Checked)
		{
			std::cout << "axis enabled" << std::endl; 
			renderViewport.axisObject->SetVisible(true);
			renderViewport.update();
		}
		else if(state == Qt::Unchecked)		
		{
			std::cout << "axis disabled" << std::endl; 
			renderViewport.axisObject->SetVisible(false);
			renderViewport.update();
		}
	});
	
	QObject::connect(controlPanel.comboRenderer, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [this](const QString &text)
	{
		if(text == QString("Slice"))
		{
			renderViewport.ChooseRenderer(RenderViewport::SLICE_RENDER);
		}
		else if(text == QString("Ray"))
		{
			renderViewport.ChooseRenderer(RenderViewport::RAY_RENDER);
		}
		else if(text == QString("Photon"))
		{
			renderViewport.ChooseRenderer(RenderViewport::PHOTON_RENDER);
		}
	});
	
	QObject::connect(controlPanel.imageSettings->sampleMapping, &SampleMappingEditor::CurveChanged, [this]()
	{
		int sizeLUT = 256; 
		std::vector<float> bufferLUT(sizeLUT * 4);
		controlPanel.imageSettings->sampleMapping->GetLUT(sizeLUT, &bufferLUT[0]);
		renderViewport.LoadLUT(&bufferLUT[0], sizeLUT);
		
	});
	
	QObject::connect(controlPanel.imageSettings->chooserGradientThreshold, &ScalarChooser::valueChanged, [this](float value)
	{
		renderViewport.SetGradientThreshold(value);
	});
	
	QObject::connect(controlPanel.imageSettings->checkBackFaceCulling, &QCheckBox::stateChanged, [this](int state)
	{
		bool value = state == Qt::Checked; 
		renderViewport.SetBackFaceCulling(value);
	});
	
	
	//central
	setCentralWidget(&renderViewport);

	//dockables
	addDockWidget(Qt::LeftDockWidgetArea, &controlPanel);
	
	showNormal();
	ExpandToFitScreen();
}

void MainWindow::ExpandToFitScreen()
{
	//Expand the window to fill the avalible screen space
	QRect rec = QApplication::desktop()->availableGeometry(this);
	int differenceH = frameGeometry().height() - geometry().height();
	int differenceW = frameGeometry().width() - geometry().width();
	int height = rec.height() - differenceH;
	int width = rec.width() - differenceW;
	resize(width, height);
	move(rec.topLeft().x(), rec.topLeft().y());	
}

void MainWindow::Save()
{
	std::cout << "Save" << std::endl;
}

void MainWindow::Load()
{
	std::cout << "Load" << std::endl;
}
