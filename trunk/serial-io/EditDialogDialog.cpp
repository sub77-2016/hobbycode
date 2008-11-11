/**
 * Serial-IO
 * Copyright (C) 2007-2008  Wilfried Holzke
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License only.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


#include <iostream>
#include "EditDialogDialog.h"
#include "LongLongIntValidator.h"

using namespace std;

EditDialogDialog::EditDialogDialog(Dialog* dlg, bool EditMode,  QList<Dialog> *DialogList) {
	
	m_lastSelectedVariable = -1;
	
	m_Dialog = dlg;
	for (unsigned int i=0; i<dlg->getVariableCount(); i++) {
		Variable *var = new Variable();
		*var = dlg->getVariable(i);
		m_Variables.push_back(var);
	}
	
	m_EditMode = EditMode;
	m_DialogList = DialogList;
	
	loadConfig();
	
	if (m_EditMode == true) setWindowTitle(tr("New Dialog"));
	else setWindowTitle(tr("Edit Dialog"));
	
    QVBoxLayout *MainLayout = new QVBoxLayout();
	setLayout(MainLayout);	
	
	// ***** sub layout ***
	
	QGridLayout *GridLayout = new QGridLayout();
	MainLayout->addLayout(GridLayout);

	// ***** Name  ********
	
	QHBoxLayout *NameLayout = new QHBoxLayout();
	GridLayout->addLayout(NameLayout,0,0);
	
	NameLayout->addWidget(new QLabel("Dialogname:"));
	
	m_TFName = new QLineEdit();
	m_TFName->setText(dlg->Name());
	connect(m_TFName,SIGNAL(textChanged(const QString &)),this,SLOT(DialogNameChanged(const QString &)));
	NameLayout->addWidget(m_TFName);
	
	QStringList TypeList;
	TypeList << "bool";
	TypeList << "string";
	TypeList << "integer";

	QHBoxLayout *VarButtonLayout = new QHBoxLayout();
	GridLayout->addLayout(VarButtonLayout,1,0);

	VarButtonLayout->addWidget(new QLabel("Variable:"));
	
	m_TypeCB = new QComboBox();
	m_TypeCB->addItems(TypeList);
	VarButtonLayout->addWidget(m_TypeCB);
	
	m_AddTypeButton = new QPushButton("Add");
	connect(m_AddTypeButton,SIGNAL(clicked()),this,SLOT(addVariable()));
	VarButtonLayout->addWidget(m_AddTypeButton);
	VarButtonLayout->addStretch(1);
	

	// ***** variable list *****
	
	QStringList VarHeader;
	VarHeader << "Name";
	VarHeader << "Type";
	
	m_LWVariables = new QTableWidget(0,VarHeader.size(),this);
	m_LWVariables->setHorizontalHeaderLabels(VarHeader);
	m_LWVariables->setGridStyle(Qt::NoPen);
	m_LWVariables->setSelectionMode(QAbstractItemView::SingleSelection);
	m_LWVariables->setSelectionBehavior(QAbstractItemView::SelectRows);
	
	for (int i=0; i<m_Variables.size(); i++) {
		QString s;
		if (m_Variables.at(i)->Type() == Variable::STRING) {
			s = "string";
		} else if (m_Variables.at(i)->Type() == Variable::BOOL) {
			s = "bool";
		} else if (m_Variables.at(i)->Type() == Variable::INTEGER) {
			s = "integer";
		}
		addTableRow(m_Variables.at(i)->Name(),s);
	}
	m_LWVariables->resizeColumnsToContents();
	
	connect(m_LWVariables,SIGNAL(itemSelectionChanged()),this,SLOT(selectedVariableChanged()));
	connect(m_LWVariables,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemChanged(QTableWidgetItem*)));
	GridLayout->addWidget(m_LWVariables,2,0);

	
	// ****** list buttons
	QHBoxLayout *ListButtonLayout = new QHBoxLayout();
	GridLayout->addLayout(ListButtonLayout,3,0);
	
	ListButtonLayout->addStretch(1);
	m_VarUpButton = new QPushButton("Up");
	m_VarUpButton->setEnabled(false);
	connect(m_VarUpButton,SIGNAL(clicked()),this,SLOT(VariableUp()));
	ListButtonLayout->addWidget(m_VarUpButton);
	
	m_VarDownButton = new QPushButton("Down");
	m_VarDownButton->setEnabled(m_Variables.size() > 0);
	connect(m_VarDownButton,SIGNAL(clicked()),this,SLOT(VariableDown()));
	ListButtonLayout->addWidget(m_VarDownButton);
	
	m_RemTypeButton = new QPushButton("Remove");
	m_RemTypeButton->setEnabled(m_Variables.size() > 0);
	connect(m_RemTypeButton,SIGNAL(clicked()),this,SLOT(remVariable()));
	ListButtonLayout->addWidget(m_RemTypeButton);
	
	
	// ***** representation *****
	
	QVBoxLayout *RepresentLayout = new QVBoxLayout();
	GridLayout->addLayout(RepresentLayout,1,1,2,1);
	
	QHBoxLayout *RepCBLayout = new QHBoxLayout();
	RepresentLayout->addLayout(RepCBLayout);
	
	RepCBLayout->addWidget(new QLabel("Representation:"));
	m_RepresentCB = new QComboBox();
	m_RepresentCB->setEnabled(false);
	connect(m_RepresentCB,SIGNAL(currentIndexChanged(int)),this,SLOT(RepresentationChanged(int)));
	RepCBLayout->addWidget(m_RepresentCB);

	m_RepresentArea = new QScrollArea();
	m_RepresentArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	RepresentLayout->addWidget(m_RepresentArea);
	
	

	// ***** Buttons *****
	m_ButtonBox = new QDialogButtonBox();

	QString AddButtonLabel;
	
	if (m_EditMode == true) AddButtonLabel = "Add";
	else AddButtonLabel = "Change";

	m_AddButton = new QPushButton(AddButtonLabel);
	m_AddButton->setEnabled(dlg->Name().length() > 0);
	m_ButtonBox->addButton(m_AddButton,QDialogButtonBox::AcceptRole);
    connect(m_ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	
	m_CancelButton = new QPushButton("Cancel");                       
	m_ButtonBox->addButton(m_CancelButton,QDialogButtonBox::RejectRole);
    connect(m_ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    MainLayout->addWidget(m_ButtonBox);
    
    if (m_Variables.size() > 0) {
        m_LWVariables->selectRow(0);
    }
}

EditDialogDialog::~EditDialogDialog() {
	saveConfig();
	
	for (int i=0; i<m_Variables.size(); i++) {
		delete m_Variables.at(i);
	}
}

void EditDialogDialog::accept() {
	
	m_Dialog->setName(m_TFName->text());
	
	saveCurrentVariableData();

	m_Dialog->clearVariables();
	for (int i=0; i<m_Variables.size(); i++) {
		m_Dialog->addVariable(*m_Variables.at(i));
	}
	
	QDialog::accept();
}

void EditDialogDialog::addVariable() {
	
	Variable *var = new Variable();

	var->setName("name");
	
	Representation r;
	r.setType(Representation::TEXTFIELD);
	var->setRepresentation(r);
	
	if (m_TypeCB->currentText() == "bool") {
		var->setType(Variable::BOOL);
	} else 	if (m_TypeCB->currentText() == "integer") {
		var->setType(Variable::INTEGER);
	} else if (m_TypeCB->currentText() == "string") {
		var->setType(Variable::STRING);
	}
	
	m_Variables.push_back(var);
	
	addTableRow("name",m_TypeCB->currentText());
	
	m_LWVariables->selectRow(m_LWVariables->rowCount()-1);
	m_LWVariables->resizeColumnsToContents();
	updateButtonStates();
}

void EditDialogDialog::remVariable() {
	
	int idx=0;
	if (getSelectedVariableRow(idx)) {

		m_lastSelectedVariable = -1;

		delete m_Variables.at(idx);
		m_Variables.removeAt(idx);
		m_LWVariables->removeRow(idx);
		
		if (m_LWVariables->rowCount() > 0) {
			if (idx > (m_LWVariables->rowCount()-1)) idx--;
			m_LWVariables->selectRow(idx);
		} 
	}
}

void EditDialogDialog::itemChanged(QTableWidgetItem * item) {

	if (item->column() == 0) {
		m_Variables.at(item->row())->setName(item->text());
		m_LWVariables->resizeColumnsToContents();
	}
}

void EditDialogDialog::selectedVariableChanged() {
	
	if (m_lastSelectedVariable != -1) {
		saveVariableData(m_lastSelectedVariable);
	}
	
	QList<QTableWidgetItem*> itemList = m_LWVariables->selectedItems();
	
	disconnectRepresentationCB();
	m_RepresentCB->clear();
	
	int row = -1;
	
	if (itemList.size() > 0) {
		
		row = itemList.at(0)->row();
		
		m_CurrentType = m_LWVariables->item(row,1)->data(Qt::DisplayRole).toString();
		
		fillRepresentationCB(m_CurrentType);
		
		QString RStr;
		if (m_Variables.at(row)->Represent().Type() == Representation::TEXTFIELD) {
			RStr = "Textfield";
		} else if (m_Variables.at(row)->Represent().Type() == Representation::CHECKBOX) {
			RStr = "CheckBox";
		} else if (m_Variables.at(row)->Represent().Type() == Representation::RADIOBUTTON) {
			RStr = "RadioButton";
		} 
		
		int idx = 0;
		for (int i=0; i<m_RepresentCB->count();i++) {
			if (m_RepresentCB->itemText(i) == RStr) {
				idx = i;
				break;
			}
		}
		
		m_RepresentCB->setCurrentIndex(idx);
		createRepresentationFrameWidgets(RStr);
		
		restoreVariableData(row);
	}
	m_lastSelectedVariable = row;
	m_RepresentCB->setEnabled(itemList.size() > 0);
	connectRepresentationCB();
}

void EditDialogDialog::DialogNameChanged(const QString &text) {
	
	bool nameExist=false;

	for (int i=0; i<m_DialogList->size(); i++) {
		if (text == m_DialogList->at(i).Name()) {
			nameExist = true;
			break;
		}
	}
	
	m_AddButton->setEnabled((text.length() > 0) && (!nameExist));
}

void EditDialogDialog::VariableUp() {

	int idx=0;
	if (getSelectedVariableRow(idx)) {

		QString name = m_LWVariables->item(idx,0)->data(Qt::DisplayRole).toString();
		QString type = m_LWVariables->item(idx,1)->data(Qt::DisplayRole).toString();
		
		m_LWVariables->removeRow(idx);
		
		m_Variables.move(idx,idx-1);

		insertTableRow(idx-1,name,type);
		
		m_LWVariables->selectRow(idx-1);
		
		updateButtonStates();
	}
}

void EditDialogDialog::VariableDown() {

	int idx=0;
	if (getSelectedVariableRow(idx)) {

		QString name = m_LWVariables->item(idx,0)->data(Qt::DisplayRole).toString();
		QString type = m_LWVariables->item(idx,1)->data(Qt::DisplayRole).toString();
		
		m_LWVariables->removeRow(idx);
		
		m_Variables.move(idx,idx+1);

		insertTableRow(idx+1,name,type);
		
		m_LWVariables->selectRow(idx+1);
		
		updateButtonStates();
	}
}

void EditDialogDialog::RepresentationChanged(int index) {
	
	createRepresentationFrameWidgets(m_RepresentCB->itemText(index));
	
	int idx=0;
	if (getSelectedVariableRow(idx)) {
	
		Representation r;
		
		if (m_RepresentCB->itemText(index) == "Textfield") {
			r.setType(Representation::TEXTFIELD);
		} else if (m_RepresentCB->itemText(index) == "CheckBox") {
			r.setType(Representation::CHECKBOX);
		} else if (m_RepresentCB->itemText(index) == "RadioButton") {
			r.setType(Representation::RADIOBUTTON);
		} 
		
		m_Variables.at(idx)->setRepresentation(r);
		m_NameLineEdit->setText(m_Variables.at(idx)->Name());
	}
}

void EditDialogDialog::fillRepresentationCB(QString type) {

	QStringList RList;
	
	if (type == "string") {
		RList << "Textfield";
	} else if (type == "bool") {
		RList << "Textfield";
		RList << "RadioButton";
		RList << "CheckBox";
	} else if (type == "integer") {
		RList << "Textfield";
		RList << "CheckBox";
	}
	m_RepresentCB->addItems(RList);
}

void EditDialogDialog::disconnectRepresentationCB() {
	disconnect(m_RepresentCB,SIGNAL(currentIndexChanged(int)),this,SLOT(RepresentationChanged(int)));	
}

void EditDialogDialog::connectRepresentationCB() {
	connect(m_RepresentCB,SIGNAL(currentIndexChanged(int)),this,SLOT(RepresentationChanged(int)));	
}

void EditDialogDialog::addTableRow(QString name, QString type) {

	int row = m_LWVariables->rowCount();
	m_LWVariables->insertRow(row);
	
	QTableWidgetItem *item = new QTableWidgetItem();
	QVariant n(name);
	item->setData(Qt::DisplayRole,n);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));
	m_LWVariables->setItem(row,0,item);
	
	item = new QTableWidgetItem();
	QVariant t = QString(type);
	item->setData(Qt::DisplayRole,t);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));
	m_LWVariables->setItem(row,1,item);
}

void EditDialogDialog::insertTableRow(unsigned int row, QString name, QString type) {

	m_LWVariables->insertRow(row);
	
	QTableWidgetItem *item = new QTableWidgetItem();
	QVariant n(name);
	item->setData(Qt::DisplayRole,n);
	m_LWVariables->setItem(row,0,item);
	
	item = new QTableWidgetItem();
	QVariant t = QString(type);
	item->setData(Qt::DisplayRole,t);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));
	m_LWVariables->setItem(row,1,item);
}

void EditDialogDialog::updateButtonStates() {

	m_RemTypeButton->setEnabled(m_LWVariables->rowCount() > 0);
	
	int idx=0;
	m_VarUpButton->setEnabled(getSelectedVariableRow(idx) && (idx > 0));
	m_VarDownButton->setEnabled(getSelectedVariableRow(idx) && (idx < (m_LWVariables->rowCount()-1)));
}

bool EditDialogDialog::getSelectedVariableRow(int &idx) {
	
	bool result=false; 
	
	QList<QTableWidgetItem*> list = m_LWVariables->selectedItems();
	
	if (list.size() > 0) {
		
		idx = list.at(0)->row();
		
		result = true;
	}
	return result;
}

void EditDialogDialog::createRepresentationFrameWidgets(QString r) {

	if (r == "Textfield") {
		createTextfield();
	} else if (r == "RadioButton") {
		createRadioButton();
	} else if (r == "CheckBox") {
		createCheckBox();
	}
}

void EditDialogDialog::createTextfield() {
	
	m_RepresentFrame = new QFrame();
	
	QVBoxLayout *MainLayout = new QVBoxLayout();
	m_RepresentFrame->setLayout(MainLayout);

	QHBoxLayout *NameLayout = new QHBoxLayout();
	MainLayout->addLayout(NameLayout);
	
	NameLayout->addWidget(new QLabel("Name:"));
	m_NameLineEdit = new QLineEdit();
	NameLayout->addWidget(m_NameLineEdit);
	connect(m_NameLineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(changedVariableName(const QString &)));
	
	if (m_CurrentType == "string") {
		
		QHBoxLayout *ConstLayout = new QHBoxLayout();
		MainLayout->addLayout(ConstLayout);
		
		m_ConstRadioButton = new QRadioButton("constant length: ");
		m_ConstRadioButton->setChecked(true);
		ConstLayout->addWidget(m_ConstRadioButton);

		m_ConstLineEdit = new QLineEdit();
		QValidator *CLValidator = new QIntValidator(1, 1024, this);
		m_ConstLineEdit->setValidator(CLValidator);
		ConstLayout->addWidget(m_ConstLineEdit);
		
		QHBoxLayout *ZeroLayout = new QHBoxLayout();
		MainLayout->addLayout(ZeroLayout);
		
		m_ZeroRadioButton = new QRadioButton("zero terminated, max length: ");
		ZeroLayout->addWidget(m_ZeroRadioButton);
		
		m_ZeroLineEdit = new QLineEdit();
		QValidator *ZTValidator = new QIntValidator(1, 1024, this);
		m_ZeroLineEdit->setValidator(ZTValidator);
		ZeroLayout->addWidget(m_ZeroLineEdit);
		
		m_STypeGroup = new QButtonGroup(m_RepresentFrame);
		m_STypeGroup->addButton(m_ConstRadioButton,0);
		m_STypeGroup->addButton(m_ZeroRadioButton,1);
		
		
	} else if (m_CurrentType == "integer") {

		MainLayout->addWidget(new QLabel("Size:"));
		
		m_Size0Button = new QRadioButton("8 Bit");
		MainLayout->addWidget(m_Size0Button);
		m_Size1Button = new QRadioButton("16 Bit");
		MainLayout->addWidget(m_Size1Button);
		m_Size2Button = new QRadioButton("32 Bit");
		MainLayout->addWidget(m_Size2Button);

		m_SizeGroup = new QButtonGroup(m_RepresentFrame);
		m_SizeGroup->addButton(m_Size0Button,0);
		m_SizeGroup->addButton(m_Size1Button,1);
		m_SizeGroup->addButton(m_Size2Button,2);
		
		connect(m_Size0Button,SIGNAL(toggled(bool)),this,SLOT(changedLimits(bool)));
		connect(m_Size1Button,SIGNAL(toggled(bool)),this,SLOT(changedLimits(bool)));
		connect(m_Size2Button,SIGNAL(toggled(bool)),this,SLOT(changedLimits(bool)));

		MainLayout->addWidget(new QLabel("Sign:"));
		
		m_Sign0Button = new QRadioButton("unsigned");
		MainLayout->addWidget(m_Sign0Button);
		m_Sign1Button = new QRadioButton("signed");
		MainLayout->addWidget(m_Sign1Button);

		m_SignGroup = new QButtonGroup(m_RepresentFrame);
		m_SignGroup->addButton(m_Sign0Button,0);
		m_SignGroup->addButton(m_Sign1Button,1);

		connect(m_Sign0Button,SIGNAL(toggled(bool)),this,SLOT(changedLimits(bool)));
		connect(m_Sign1Button,SIGNAL(toggled(bool)),this,SLOT(changedLimits(bool)));
		
		
		MainLayout->addWidget(new QLabel("Limits:"));

		QHBoxLayout *LowerLayout = new QHBoxLayout();
		MainLayout->addLayout(LowerLayout);
		LowerLayout->addWidget(new QLabel(" - lower: "));
		m_LowerLineEdit = new QLineEdit();
		m_LowerLineEdit->setValidator(new LongLongIntValidator(0,0,this));		
		LowerLayout->addWidget(m_LowerLineEdit);
		
		m_LowerLabel = new QLabel("(0..0)");
		LowerLayout->addWidget(m_LowerLabel);

		QHBoxLayout *UpperLayout = new QHBoxLayout();
		MainLayout->addLayout(UpperLayout);
		UpperLayout->addWidget(new QLabel(" - upper: "));
		m_UpperLineEdit = new QLineEdit();
		m_UpperLineEdit->setValidator(new LongLongIntValidator(0,0,this));		
		UpperLayout->addWidget(m_UpperLineEdit);
		
		m_UpperLabel = new QLabel("(0..0)");
		UpperLayout->addWidget(m_UpperLabel);
	}
	
	m_RepresentArea->setWidget(m_RepresentFrame);
	m_RepresentFrame->show();
}

void EditDialogDialog::createRadioButton() {
	
	m_RepresentFrame = new QFrame();
	
	QVBoxLayout *MainLayout = new QVBoxLayout();
	m_RepresentFrame->setLayout(MainLayout);

	QHBoxLayout *NameLayout = new QHBoxLayout();
	MainLayout->addLayout(NameLayout);
	
	NameLayout->addWidget(new QLabel("Name:"));
	m_NameLineEdit = new QLineEdit();
	NameLayout->addWidget(m_NameLineEdit);
	connect(m_NameLineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(changedVariableName(const QString &)));
	
	QHBoxLayout *OffLayout = new QHBoxLayout();
	MainLayout->addLayout(OffLayout);
	
	m_OffRadioButton = new QRadioButton("Text: ");
	m_OffRadioButton->setEnabled(false);
	OffLayout->addWidget(m_OffRadioButton);
	
	m_OffLineEdit = new QLineEdit();
	OffLayout->addWidget(m_OffLineEdit);
	
	QHBoxLayout *OnLayout = new QHBoxLayout();
	MainLayout->addLayout(OnLayout);
	
	m_OnRadioButton = new QRadioButton("Text: ");
	m_OnRadioButton->setEnabled(false);
	m_OnRadioButton->setChecked(true);
	OnLayout->addWidget(m_OnRadioButton);

	m_OnLineEdit = new QLineEdit();
	OnLayout->addWidget(m_OnLineEdit);

	m_RepresentArea->setWidget(m_RepresentFrame);
	m_RepresentFrame->show();
}

void EditDialogDialog::createCheckBox() {
	
	m_RepresentFrame = new QFrame();
	
	QVBoxLayout *MainLayout = new QVBoxLayout();
	m_RepresentFrame->setLayout(MainLayout);

	QHBoxLayout *NameLayout = new QHBoxLayout();
	MainLayout->addLayout(NameLayout);
	
	NameLayout->addWidget(new QLabel("Name:"));
	m_NameLineEdit = new QLineEdit();
	NameLayout->addWidget(m_NameLineEdit);
	connect(m_NameLineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(changedVariableName(const QString &)));
	
	if (m_CurrentType == "integer") {

		MainLayout->addWidget(new QLabel("Size:"));
		
		m_Size0Button = new QRadioButton("8 Bit");
		m_Size0Button->setChecked(true);
		MainLayout->addWidget(m_Size0Button);
		m_Size1Button = new QRadioButton("16 Bit");
		MainLayout->addWidget(m_Size1Button);
		m_Size2Button = new QRadioButton("32 Bit");
		MainLayout->addWidget(m_Size2Button);

		QButtonGroup *SizeGroup = new QButtonGroup(m_RepresentFrame);
		SizeGroup->addButton(m_Size0Button,0);
		SizeGroup->addButton(m_Size1Button,1);
		SizeGroup->addButton(m_Size2Button,2);
		
		MainLayout->addWidget(new QLabel("Bit-Text:"));

		m_BitTextLineEdit.clear();
		for (int i=0; i<32; i++) {
			
			QHBoxLayout *hLayout = new QHBoxLayout();
			MainLayout->addLayout(hLayout);
			
			hLayout->addWidget(new QLabel("Bit "+QString("%0").arg(i)+": "));
			
			QLineEdit *BitTextLineEdit = new QLineEdit();
			hLayout->addWidget(BitTextLineEdit);
			m_BitTextLineEdit.push_back(BitTextLineEdit);
		}
	}

	m_RepresentArea->setWidget(m_RepresentFrame);
	m_RepresentFrame->show();
}

void EditDialogDialog::saveCurrentVariableData() {
	
	int idx;
	if (getSelectedVariableRow(idx)) {

		saveVariableData(idx);
	}
}

void EditDialogDialog::saveVariableData(int idx) {

	Representation r = m_Variables.at(idx)->Represent();

	if (m_RepresentCB->currentText() == "Textfield") {

		if (m_CurrentType == "string") {
			r.setAttribute("StringType",QString("%1").arg(m_STypeGroup->checkedId()));
			r.setAttribute("ConstLength",m_ConstLineEdit->text());
			r.setAttribute("ZeroTerminatedMaxLength",m_ZeroLineEdit->text());
			
		} else if (m_CurrentType == "integer") {
			if (m_Size0Button->isChecked()) r.setAttribute("Size","8");
			else if (m_Size1Button->isChecked()) r.setAttribute("Size","16");
			else if (m_Size2Button->isChecked()) r.setAttribute("Size","32");
			
			r.setAttribute("Signed",QString("%1").arg(m_SignGroup->checkedId()));
			r.setAttribute("LowerLimit",m_LowerLineEdit->text());
			r.setAttribute("UpperLimit",m_UpperLineEdit->text());
		}
	} else if (m_RepresentCB->currentText() == "CheckBox") {

		if (m_CurrentType == "integer") {
			
			if (m_Size0Button->isChecked()) r.setAttribute("Size","8");
			else if (m_Size1Button->isChecked()) r.setAttribute("Size","16");
			else if (m_Size2Button->isChecked()) r.setAttribute("Size","32");
			
			for (int i=0; i<32; i++) {
				r.setAttribute("BitText_"+QString("%1").arg(i),m_BitTextLineEdit.at(i)->text());
			}
		}
	} else if (m_RepresentCB->currentText() == "RadioButton") {

		r.setAttribute("RadioButtonOnText",m_OnLineEdit->text());
		r.setAttribute("RadioButtonOffText",m_OffLineEdit->text());
	}
	m_Variables.at(idx)->setRepresentation(r);
}

void EditDialogDialog::restoreVariableData(int idx) {
	
	Representation r = m_Variables.at(idx)->Represent();

	m_NameLineEdit->setText(m_Variables.at(idx)->Name());

	if (m_RepresentCB->currentText() == "Textfield") {

		if (m_CurrentType == "string") {
			
			if (r.Attribute("StringType") == "0") m_ConstRadioButton->setChecked(true);
			else m_ZeroRadioButton->setChecked(true);

			QString a = r.Attribute("ConstLength");
			if (a.length() == 0) a = "8";
			m_ConstLineEdit->setText(a);
			
			a = r.Attribute("ZeroTerminatedMaxLength");
			if (a.length() == 0) a = "128";
			m_ZeroLineEdit->setText(a);
			
		} else if (m_CurrentType == "integer") {
			if ((r.Attribute("Size") == "8") || (r.Attribute("Size") == "")) m_Size0Button->setChecked(true);
			else if (r.Attribute("Size") == "16") m_Size1Button->setChecked(true);
			else if (r.Attribute("Size") == "32") m_Size2Button->setChecked(true);
			
			if ((r.Attribute("Signed") == "0") || (r.Attribute("Signed") == "")) m_Sign0Button->setChecked(true);
			else m_Sign1Button->setChecked(true);
			
			QString a = r.Attribute("LowerLimit");
			if (a.length() == 0) {
				if (m_Sign0Button->isChecked()) {
					a = "0";
				} else {
					if (m_Size0Button->isChecked()) a = "-128";
					else if (m_Size1Button->isChecked()) a = "-16384";
					else if (m_Size2Button->isChecked()) a = "-2147483648";
				}
			}
			m_LowerLineEdit->setText(a);
			
			a = r.Attribute("UpperLimit");
			if (a.length() == 0) {
				if (m_Sign0Button->isChecked()) {
					if (m_Size0Button->isChecked()) a = "255";
					else if (m_Size1Button->isChecked()) a = "65535";
					else if (m_Size2Button->isChecked()) a = "4294967296";
				} else {
					if (m_Size0Button->isChecked()) a = "127";
					else if (m_Size1Button->isChecked()) a = "16383";
					else if (m_Size2Button->isChecked()) a = "2147483647";
				}
			}
			m_UpperLineEdit->setText(a); 
		}
	} else if (m_RepresentCB->currentText() == "CheckBox") {

		if (m_CurrentType == "integer") {
			
			if (r.Attribute("Size") == "8") m_Size0Button->setChecked(true);
			else if (r.Attribute("Size") == "16") m_Size1Button->setChecked(true);
			else if (r.Attribute("Size") == "32") m_Size2Button->setChecked(true);
			
			for (int i=0; i<32; i++) {
				m_BitTextLineEdit.at(i)->setText(r.Attribute("BitText_"+QString("%1").arg(i)));
			} 
		}
	} else if (m_RepresentCB->currentText() == "RadioButton") {

		m_OnLineEdit->setText(r.Attribute("RadioButtonOnText"));
		m_OffLineEdit->setText(r.Attribute("RadioButtonOffText"));
	}
}

void EditDialogDialog::changedLimits(bool toggled) {
	
	if (!toggled) return;
	
	LongLongIntValidator *lv = (LongLongIntValidator*) m_LowerLineEdit->validator();
	LongLongIntValidator *uv = (LongLongIntValidator*) m_UpperLineEdit->validator();
	
	if (m_Sign0Button->isChecked()) {
		// unsigned
		
		if (m_Size0Button->isChecked()) {
			lv->setRange(0,255);
			uv->setRange(0,255);

			m_UpperLabel->setText("(0..255)");
			m_LowerLabel->setText("(0..255)");
			
			if (!ValueOk(m_LowerLineEdit->text(),0,255)) m_LowerLineEdit->setText("0");
			if (!ValueOk(m_UpperLineEdit->text(),0,255)) m_UpperLineEdit->setText("255");

		} else if (m_Size1Button->isChecked()) {
			lv->setRange(0,65535);
			uv->setRange(0,65535);

			m_UpperLabel->setText("(0..65535)");
			m_LowerLabel->setText("(0..65535)");

			if (!ValueOk(m_LowerLineEdit->text(),0,65536)) m_LowerLineEdit->setText("0");
			if (!ValueOk(m_UpperLineEdit->text(),0,65536)) m_UpperLineEdit->setText("65535");
			
		} else if (m_Size2Button->isChecked()) {
			lv->setRange(0,4294967295LL);
			uv->setRange(0,4294967295LL);

			m_UpperLabel->setText("(0..4294967295)");
			m_LowerLabel->setText("(0..4294967295)");

			if (!ValueOk(m_LowerLineEdit->text(),0,4294967295LL)) m_LowerLineEdit->setText("0");
			if (!ValueOk(m_UpperLineEdit->text(),0,4294967295LL)) m_UpperLineEdit->setText("4294967295");
		}
	} else {
		// signed
		if (m_Size0Button->isChecked()) {
			lv->setRange(-128,127);
			uv->setRange(-128,127);

			m_UpperLabel->setText("(-128..127)");
			m_LowerLabel->setText("(-128..127)");

			if (!ValueOk(m_LowerLineEdit->text(),-128,127)) m_LowerLineEdit->setText("-128");
			if (!ValueOk(m_UpperLineEdit->text(),-128,127)) m_UpperLineEdit->setText("127");
			
		} else if (m_Size1Button->isChecked()) {
			lv->setRange(-32768,32767);
			uv->setRange(-32768,32767);

			m_UpperLabel->setText("(-32768..32767)");
			m_LowerLabel->setText("(-32768..32767)");

			if (!ValueOk(m_LowerLineEdit->text(),-32768,32767)) m_LowerLineEdit->setText("-32768");
			if (!ValueOk(m_UpperLineEdit->text(),-32768,32767)) m_UpperLineEdit->setText("32767");
			
		} else if (m_Size2Button->isChecked()) {
			lv->setRange(-2147483648LL,2147483647);
			uv->setRange(-2147483648LL,2147483647);

			m_UpperLabel->setText("(-2147483648..2147483647)");
			m_LowerLabel->setText("(-2147483648..2147483647)");

			if (!ValueOk(m_LowerLineEdit->text(),-2147483648LL,2147483647)) m_LowerLineEdit->setText("-2147483648");
			if (!ValueOk(m_UpperLineEdit->text(),-2147483648LL,2147483647)) m_UpperLineEdit->setText("2147483647");
		}
	}
	m_RepresentFrame->adjustSize();
}

void EditDialogDialog::changedVariableName(const QString &text) {
	
	QList<QTableWidgetItem*> itemList = m_LWVariables->selectedItems();
	
	if (itemList.size() != 0) {
	
		m_LWVariables->item(itemList.at(0)->row(),0)->setText(text);
	}
}

bool EditDialogDialog::ValueOk(QString input, long long int min, long long int max) {

	bool result = false;
	
	bool ok = false;
	
	long long int i = input.toLongLong(&ok,10);
	
	if (ok) {
		if ((i >= min) && (i <= max)) {
			result = true;
		}
	}
	
	return result;
}

void EditDialogDialog::saveConfig() {
	
	QSettings settings;
	
	settings.beginGroup("EditDialogDialog");
	settings.setValue("pos",pos());
	settings.endGroup();
}

void EditDialogDialog::loadConfig() {
	
	QSettings settings;
	
	settings.beginGroup("EditDialogDialog");
	if (settings.contains("pos")) {
		QPoint pos = settings.value("pos").toPoint();
		move(pos);
	}
	settings.endGroup();
}
