/*
 * Copyright (C) 2017 ~ 2018 Wuhan Deepin Technology Co., Ltd.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "usbselectview.h"
#include "widgetutil.h"
#include "deviceinfoitem.h"
#include "devicelistwidget.h"
#include <bminterface.h>

#include <DListWidget>
#include <DLabel>
#include <DCheckBox>
#include <DPalette>
#include <DCheckBox>
#include <DPushButton>
#include <DApplicationHelper>
#include <DFontSizeManager>
#include <DDialog>

#include <QDebug>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QIcon>
#include <QFontDatabase>

static QString usageString(quint32 usage, quint32 total)
{
    if (total <= 0 || usage > total) {
        return "0/0G";
    }

    if (total <= 1024) {
        return QString("%1/%2M").arg(usage).arg(total);
    }

    return QString("%1/%2G")
           .arg(QString::number(static_cast<double>(usage) / 1024, 'f', 2))
           .arg(QString::number(static_cast<double>(total) / 1024, 'f', 2));
}


static int percent(quint32 usage, quint32 total)
{
    if (total <= 0) {
        return 0;
    }

    if (usage > total) {
        return 100;
    }

    return static_cast<int>(usage * 100 / total);
}

UsbSelectView::UsbSelectView(DWidget *parent) : DWidget(parent)
{
    setObjectName("UsbSelectView");
    setAutoFillBackground(true);
//    int lcdFontId = QFontDatabase::addApplicationFont(":/theme/SourceHanSansSC-Medium.ttf");
//    QStringList m_fontList;
//    m_fontList.clear();
//    if (lcdFontId != -1) {
//        m_fontList << QFontDatabase::applicationFontFamilies(lcdFontId);
//    }
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 1, 0, 0);

    DLabel *m_title = new DLabel(tr("Select a disk"));
    DPalette pa = DApplicationHelper::instance()->palette(m_title);
    QBrush brush = DApplicationHelper::instance()->palette(m_title).text();
    pa.setBrush(DPalette::Text, brush);
    m_title->setPalette(pa);
    m_title->setFixedHeight(36);
    DFontSizeManager::instance()->bind(m_title, DFontSizeManager::T3);
//    QFont ft = m_title->font();
//    if (m_fontList.size() > 0)
//        ft.setFamily(m_fontList.at(0));
//    ft.setPixelSize(24);
//    m_title->setFont(ft);

    DFrame *usbDeviceListPanel = new DFrame;
    usbDeviceListPanel->setObjectName("UsbDeviceListPanel");
    usbDeviceListPanel->setFixedSize(410, 302);
    usbDeviceListPanel->setFrameStyle(QFrame::NoFrame);
    usbDeviceListPanel->setAutoFillBackground(true);

    QVBoxLayout *usbPanelLayout = new QVBoxLayout(usbDeviceListPanel);
    usbPanelLayout->setContentsMargins(10, 0, 10, 0);

    QHBoxLayout *checkBoxLayout = new QHBoxLayout;
    checkBoxLayout->setContentsMargins(0, 0, 0, 0);
    checkBoxLayout->setSpacing(0);

    DCheckBox *m_formatDiskCheck = new DCheckBox(this);
    m_formatDiskCheck->setText(tr("Format the disk to increase the burning success rate"));
    m_formatDiskCheck->setObjectName("UsbFormatCheckBox");
    m_formatDiskCheck->setFixedHeight(20);
    m_formatDiskCheck->setFocusPolicy(Qt::NoFocus);
    m_formatDiskCheck->hide();
    DFontSizeManager::instance()->bind(m_formatDiskCheck, DFontSizeManager::T6);
//    ft = m_formatDiskCheck->font();
//    if (m_fontList.size() > 0)
//        ft.setFamily(m_fontList.at(0));
//    ft.setPixelSize(14);
//    m_formatDiskCheck->setFont(ft);

//    QLabel *checkBoxHints = new QLabel(this);
//    checkBoxHints->setMinimumHeight(34);
//    checkBoxHints->setWordWrap(true);
//    checkBoxHints->setText(tr("Format the disk to increase the burning success rate"));
////    checkBoxHints->setStyleSheet(WidgetUtil::getQss(":/theme/light/UCheckBox.theme"));
////    checkBoxHints->setMinimumWidth(this->width());
//    ft = checkBoxHints->font();
//    ft.setPointSize(10);
//    checkBoxHints->setFont(ft);
//    checkBoxHints->hide();

    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_formatDiskCheck);
//    checkBoxLayout->addSpacing(4);
//    checkBoxLayout->addWidget(checkBoxHints);
    checkBoxLayout->addStretch();
    checkBoxLayout->setAlignment(Qt::AlignCenter);

    DeviceListWidget *m_deviceList = new DeviceListWidget;
    m_deviceList->setObjectName("UsbDeviceList");
    m_deviceList->setFixedSize(400, 270);
    m_deviceList->hide();

    DeviceDelegate *m_devicedelegate = new DeviceDelegate(m_deviceList);
    m_deviceList->setItemDelegate(m_devicedelegate);

    DLabel *m_warningHint = new  DLabel("");
    m_warningHint->setObjectName("WarningHint");
    m_warningHint->setFixedWidth(370);
    m_warningHint->setMinimumHeight(17);
    m_warningHint->setWordWrap(true);
    DFontSizeManager::instance()->bind(m_warningHint, DFontSizeManager::T9);
//    ft = m_warningHint->font();
//    ft.setPixelSize(11);
//    m_warningHint->setFont(ft);
    m_warningHint->setAlignment(Qt::AlignCenter);

    DLabel *m_emptyHint = new  DLabel(tr("No disk available"));
    m_emptyHint->setObjectName("EmptyHintTitle");
    m_emptyHint->setFixedHeight(29);
    m_emptyHint->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_emptyHint, DFontSizeManager::T4);
//    ft = m_emptyHint->font();
//    ft.setPixelSize(20);
//    m_emptyHint->setFont(ft);

    usbPanelLayout->addStretch();
    usbPanelLayout->addWidget(m_emptyHint, 0, Qt::AlignCenter);
    usbPanelLayout->addStretch();
    usbPanelLayout->addWidget(m_deviceList, 0, Qt::AlignLeft);
    usbPanelLayout->addSpacing(15);
    usbPanelLayout->addLayout(checkBoxLayout);

    DPushButton *start = new DPushButton();
    start->setFocusPolicy(Qt::NoFocus);
    start->setFixedSize(310, 36);
    start->setObjectName("StartMake");
    start->setText(tr("Start"));
    DFontSizeManager::instance()->bind(start, DFontSizeManager::T6);
//    ft = start->font();
//    if (m_fontList.size() > 0)
//        ft.setFamily(m_fontList.at(0));
//    ft.setPixelSize(14);
//    start->setFont(ft);
    start->setDisabled(true);

    mainLayout->addWidget(m_title, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(40);
    mainLayout->addWidget(usbDeviceListPanel, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_warningHint, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(start, 0, Qt::AlignHCenter);
    mainLayout->addStretch();


    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
    this, [ = ] {
        DPalette pa;
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType)
        {
            pa = palette();
            pa.setColor(DPalette::Background, QColor(255, 255, 255));
            setPalette(pa);
//            pa = m_title->palette();
//            pa.setColor(DPalette::WindowText, QColor("#414D68"));
//            m_title->setPalette(pa);
//            pa = usbDeviceListPanel->palette();
//            pa.setColor(DPalette::Background, QColor(255, 255, 255, 128));
//            usbDeviceListPanel->setPalette(pa);
            pa = m_warningHint->palette();
            pa.setColor(DPalette::WindowText, QColor("#FF5800"));
            m_warningHint->setPalette(pa);
            pa = m_emptyHint->palette();
            pa.setColor(DPalette::Text, QColor(85, 85, 85, 102));
            m_emptyHint->setPalette(pa);
        } else if (themeType == DGuiApplicationHelper::DarkType)
        {
            pa = palette();
            pa.setColor(DPalette::Background, QColor("#292929"));
            setPalette(pa);
//            pa = m_title->palette();
//            pa.setColor(DPalette::WindowText, QColor("#C0C6D4"));
//            m_title->setPalette(pa);
//            pa = usbDeviceListPanel->palette();
//            pa.setColor(DPalette::Background, QColor(0, 0, 0, 128));
//            usbDeviceListPanel->setPalette(pa);
            pa = m_warningHint->palette();
            pa.setColor(DPalette::WindowText, QColor("#9A2F2F"));
            m_warningHint->setPalette(pa);
            pa = m_emptyHint->palette();
            pa.setColor(DPalette::Text, QColor(192, 198, 212, 102));
            m_emptyHint->setPalette(pa);
        }
    });

    emit DGuiApplicationHelper::instance()->themeTypeChanged(DGuiApplicationHelper::instance()->themeType());

//    this->setStyleSheet(WidgetUtil::getQss(":/theme/light/UsbSelectView.theme"));
    auto handleFormat = [ = ](bool checked) {
        if (!checked) {
            m_warningHint->setText("");
            return;
        }
        m_warningHint->setText(tr("Formatting will erase all data on the disk, please confirm and continue"));
        this->adjustSize();
    };
    connect(m_formatDiskCheck, &DCheckBox::clicked, this, [ = ](bool checked) {
        this->setProperty("user_format", checked);
        handleFormat(checked);
    });

    connect(BMInterface::instance(), &BMInterface::deviceListChanged,
    this, [ = ](const QList<DeviceInfo> &partitions) {
        bool hasPartitionSelected = false;
        m_formatDiskCheck->setVisible(partitions.size());
//        checkBoxHints->setVisible(partitions.size());
        m_emptyHint->setVisible(!partitions.size());
        m_deviceList->setVisible(partitions.size());
//        m_formatDiskCheck->setEnabled(partitions.size());

        m_deviceList->clear();
        foreach (const DeviceInfo &partition, partitions) {
            QListWidgetItem *listItem = new QListWidgetItem;
            DeviceInfoItem *infoItem = new DeviceInfoItem(
                partition.label,
                partition.path,
                usageString(partition.used, partition.total),
                percent(partition.used, partition.total));
            infoItem->setNeedFormat(partition.needFormat);
            listItem->setSizeHint(QSize(infoItem->size().width(), infoItem->size().height() + 10));
            m_deviceList->addItem(listItem);
            m_deviceList->setItemWidget(listItem, infoItem);
            infoItem->setProperty("path", partition.path);
            infoItem->setProperty("fstype", partition.fstype);
            if (partition.path == this->property("last_path").toString()) {
                infoItem->setCheck(true);
                m_deviceList->setCurrentItem(listItem);
                hasPartitionSelected = true;
            }
        }

        start->setDisabled(!hasPartitionSelected);

        if (!hasPartitionSelected) {
            this->setProperty("last_path", "");
        }
    });

    connect(m_deviceList, &DeviceListWidget::currentItemChanged,
    this, [ = ](QListWidgetItem * current, QListWidgetItem * previous) {
        DeviceInfoItem *infoItem = qobject_cast<DeviceInfoItem *>(m_deviceList->itemWidget(previous));
        if (infoItem) {
            infoItem->setCheck(false);
        }

        infoItem = qobject_cast<DeviceInfoItem *>(m_deviceList->itemWidget(current));
        if (infoItem) {
            if (infoItem->needFormat()) {
                m_formatDiskCheck->setChecked(true);
                m_formatDiskCheck->setDisabled(true);
                handleFormat(true);
            } else {
                auto format = this->property("user_format").toBool();
                m_formatDiskCheck->setChecked(format);
                m_formatDiskCheck->setDisabled(false);
                handleFormat(format);
            }

            infoItem->setCheck(true);
            this->setProperty("last_path", infoItem->property("path").toString());
            this->setProperty("last_fstype", infoItem->property("fstype").toString());
            start->setDisabled(false);
        }
    });

    connect(start, &DPushButton::clicked, this, [ = ] {
        auto format = m_formatDiskCheck->isChecked();

        if (format)
        {
            DDialog msgbox(this);
            msgbox.setFixedWidth(400);
            msgbox.setIcon(DMessageBox::standardIcon(DMessageBox::Warning));
            msgbox.setTitle(tr("Format USB flash drive"));
            msgbox.setTextFormat(Qt::AutoText);
            msgbox.setMessage(tr("Formatting the disk will overwrite all data, please have a backup before proceeding."));
            msgbox.insertButton(0, tr("Cancel"), true, DDialog::ButtonRecommend);
            msgbox.insertButton(1, tr("OK"), false, DDialog::ButtonWarning);

            auto ret = msgbox.exec();
            if (ret != 1) {
                m_formatDiskCheck->setChecked(false);
                this->setProperty("user_format", false);
                handleFormat(false);
                return;
            }
        }

        start->setEnabled(false);
#ifdef Q_OS_LINUX
        if (!m_formatDiskCheck->isChecked() && "vfat" != this->property("last_fstype").toString())
        {
            emit finish(2, "install failed", tr("Disk Format Error: Please format the disk with FAT32"));
            return;
        }
#endif
        QString path = this->property("last_path").toString();
        qDebug() << "Select usb device" << path;
        emit this->deviceSelected(path, m_formatDiskCheck->isChecked());
    });
}

