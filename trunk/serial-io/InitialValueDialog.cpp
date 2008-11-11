#include "InitialValueDialog.h"

InitialValueDialog::InitialValueDialog()
{
	loadConfig();
	
	setWindowTitle(tr("Inital Values"));
	
    m_MainLayout = new QVBoxLayout(this);
    
    m_SValueLayout = new QHBoxLayout();
    m_MainLayout->addLayout(m_SValueLayout);
    
    m_SValueLayout->addWidget(new QLabel("Start value:"));
    m_StartSpinBox = new QSpinBox(this);
    m_StartSpinBox->setValue(0);
    m_StartSpinBox->setMinimum(0);
    m_StartSpinBox->setMaximum(65535);
    m_SValueLayout->addWidget(m_StartSpinBox); 

    m_IValueLayout = new QHBoxLayout();
    m_MainLayout->addLayout(m_IValueLayout);
    
    m_IValueLayout->addWidget(new QLabel("Increment value:"));
    m_IncSpinBox = new QSpinBox(this);
    m_IncSpinBox->setValue(0);
    m_IncSpinBox->setMinimum(0);
    m_IncSpinBox->setMaximum(65535);
    m_IValueLayout->addWidget(m_IncSpinBox); 
    
	m_MainLayout->addStretch();
	m_MainLayout->addSpacing(10);

	m_ButtonBox = new QDialogButtonBox();

	m_AddButton = new QPushButton("Ok");
    connect(m_ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	m_ButtonBox->addButton(m_AddButton,QDialogButtonBox::AcceptRole);
	
	m_CancelButton = new QPushButton("Cancel");                       
    connect(m_ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
	m_ButtonBox->addButton(m_CancelButton,QDialogButtonBox::RejectRole);
    
    m_MainLayout->addWidget(m_ButtonBox);

	setLayout(m_MainLayout);	
}

InitialValueDialog::~InitialValueDialog()
{
	saveConfig();
}


void InitialValueDialog::accept() {
	
	QDialog::accept();
}


void InitialValueDialog::closeEvent(QCloseEvent *event)
{
    if (true) {
        event->accept();
    } else {
        event->ignore();
    }
}

int InitialValueDialog::getStartValue() {
	
	return m_StartSpinBox->value();
}

int InitialValueDialog::getIncrement() {
	return m_IncSpinBox->value();	
}

void InitialValueDialog::saveConfig() {
	
	QSettings settings;
	
	settings.beginGroup("InitialValueDialog");
	settings.setValue("pos",pos());
	settings.endGroup();
}

void InitialValueDialog::loadConfig() {
	
	QSettings settings;
	
	settings.beginGroup("InitialValueDialog");
	if (settings.contains("pos")) {
		QPoint pos = settings.value("pos").toPoint();
		move(pos);
	}
	settings.endGroup();
}
