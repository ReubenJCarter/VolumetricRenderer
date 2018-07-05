#include "HelperWidgets.hpp"	
	
	
Vector3Chooser::Vector3Chooser(QString name, double defaultX, double defaultY, double defaultZ, 
					   double minX, double minY, double minZ, 
					   double maxX, double maxY, double maxZ)
{
	layout = new QHBoxLayout();
	setLayout(layout); 
	
	label = new QLabel(name);
	spinX = new QDoubleSpinBox();
	spinY = new QDoubleSpinBox();
	spinZ = new QDoubleSpinBox();
	
	layout->addWidget(label);
	layout->addWidget(spinX);
	layout->addWidget(spinY);
	layout->addWidget(spinZ);
	
	sx = defaultX;
	sy = defaultY;
	sz = defaultZ;
	
	spinX->setValue(sx);
	spinY->setValue(sy);
	spinZ->setValue(sz);
	
	spinX->setSingleStep(0.1);
	spinY->setSingleStep(0.1);
	spinZ->setSingleStep(0.1);
	
	connect(spinX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=, this](double v)
	{
		sx = v;
		emit valueChanged(sx, sy, sz);
	});
	
	connect(spinY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=, this](double v)
	{
		sy = v;
		emit valueChanged(sx, sy, sz);
	});
	
	connect(spinZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=, this](double v)
	{
		sz = v;
		emit valueChanged(sx, sy, sz);
	});
}

void Vector3Chooser::setValue(double xV, double yV, double zV)
{
	spinX->setValue(xV);
	spinY->setValue(yV);
	spinZ->setValue(zV);
}

	
ScalarChooser::ScalarChooser(QString name, double minv, double maxv, double start, double step, bool hasSlider )
{
	minVal = minv;
	maxVal = maxv;
	
	sliderStep = (maxv - minv) / 100.0;
	
	layout = new QHBoxLayout();
	setLayout(layout); 
	
	label = new QLabel(name);
	layout->addWidget(label);
	
	spin = new QDoubleSpinBox();
	layout->addWidget(spin);
	spin->setMaximum(maxv);
	spin->setMinimum(minv);
	spin->setSingleStep(step);
	spin->setValue(start);
	spin->setDecimals(4);
	
	if(hasSlider)
	{
		slider = new CustomSlider(Qt::Horizontal, (start - minv) / sliderStep);
		layout->addWidget(slider);
		slider->setMinimum(0);
		slider->setMaximum(100);
		slider->setPageStep(1);
		slider->setSingleStep(1);
		slider->setValue((start - minv) / sliderStep);
		
		connect(slider, &QSlider::valueChanged, [=, this](int v)
		{
			double newVal = minv + v * sliderStep;
			bool oldVal = spin->blockSignals(true);
			spin->setValue(newVal);
			spin->blockSignals(oldVal);
			emit valueChanged(newVal);
		});
	}
	
	connect(spin,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=, this](double v)
	{
		if(hasSlider)
		{
			bool oldVal = slider->blockSignals(true);
			slider->setValue((v - minv) / sliderStep); 
			slider->blockSignals(oldVal);
		}
		emit valueChanged(v);
	});
}

void ScalarChooser::setValue(double val)
{
	spin->setValue(minVal + val * sliderStep);
	
	//slider->setValue((val - minVal) / sliderStep); 
}

double ScalarChooser::value()
{
	return spin->value();
}