// DisplayTextSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "DisplayTextSettingDlg.h"
#include "TrafficMonitorDlg.h"
#include "SkinManager.h"


// CDisplayTextSettingDlg 对话框

IMPLEMENT_DYNAMIC(CDisplayTextSettingDlg, CBaseDialog)

CDisplayTextSettingDlg::CDisplayTextSettingDlg(DispStrings& display_texts, bool main_window_text, CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_DISPLAY_TEXT_SETTINGS_DIALOG, pParent), m_display_texts(display_texts), m_main_window_text(main_window_text)
{

}

CDisplayTextSettingDlg::~CDisplayTextSettingDlg()
{
}

void CDisplayTextSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


CString CDisplayTextSettingDlg::GetDialogName() const
{
    return _T("DisplayTextSettingDlg");
}

BEGIN_MESSAGE_MAP(CDisplayTextSettingDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_BUTTON, &CDisplayTextSettingDlg::OnBnClickedRestoreDefaultButton)
END_MESSAGE_MAP()


// CDisplayTextSettingDlg 消息处理程序


BOOL CDisplayTextSettingDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(theApp.GetMenuIcon(IDI_ITEM), FALSE);		// 设置小图标

    //初始化列表控件
    CRect rect;
    m_list_ctrl.GetClientRect(rect);
    m_list_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0, width1;
    width0 = rect.Width() / 2;
    width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);		//插入第0列
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_VALUE), LVCFMT_LEFT, width1);		//插入第1列

    //如果是主窗口，清除当前皮肤中没有的行
    if (m_main_window_text)
    {
        std::set<CommonDisplayItem> all_skin_items;
        CTrafficMonitorDlg::Instance()->GetCurSkin().GetSkinDisplayItems(all_skin_items);

        DispStrings temp = m_display_texts;
        m_display_texts = DispStrings();
        for (const auto& display_item : all_skin_items)
        {
            m_display_texts.Get(display_item) = temp.GetConst(display_item);
        }
    }

    //向列表中插入行
    for (auto iter = m_display_texts.GetAllItems().begin(); iter != m_display_texts.GetAllItems().end(); ++iter)
    {
        CString item_name = iter->first.GetItemName();
        if (!item_name.IsEmpty())
        {
            int index = m_list_ctrl.GetItemCount();
            m_list_ctrl.InsertItem(index, item_name);
            m_list_ctrl.SetItemText(index, 1, iter->second.c_str());
            m_list_ctrl.SetItemData(index, (DWORD_PTR)&(iter->first));
        }
    }

    m_list_ctrl.SetEditColMethod(CListCtrlEx::EC_SPECIFIED);        //设置列表可编辑
    m_list_ctrl.SetEditableCol({ 1 });                              //设置可编辑的列

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

CommonDisplayItem CDisplayTextSettingDlg::GetDisplayItem(int row)
{
    CommonDisplayItem* item = (CommonDisplayItem*)m_list_ctrl.GetItemData(row);
    return *item;
}

void CDisplayTextSettingDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    int item_count = m_list_ctrl.GetItemCount();
    for (int i{}; i < item_count; i++)
    {
        CommonDisplayItem display_item = GetDisplayItem(i);
        m_display_texts.Get(display_item) = m_list_ctrl.GetItemText(i, 1).GetString();
    }

    CBaseDialog::OnOK();
}



void CDisplayTextSettingDlg::OnBnClickedRestoreDefaultButton()
{
    // TODO: 在此添加控件通知处理程序代码
    int item_count = m_list_ctrl.GetItemCount();
    CTrafficMonitorDlg* pMainWnd = CTrafficMonitorDlg::Instance();
    if (m_main_window_text && pMainWnd != nullptr)
    {
        //主窗口恢复默认显示文本时，从皮肤获取
        SkinSettingData skin_setting_data;
        CSkinManager::SkinSettingDataFronSkin(skin_setting_data, pMainWnd->GetCurSkin());
        for (int i{}; i < item_count; i++)
        {
            CommonDisplayItem display_item = GetDisplayItem(i);
            std::wstring default_text = skin_setting_data.disp_str.GetConst(display_item);
            m_list_ctrl.SetItemText(i, 1, default_text.c_str());
        }
    }
    else
    {
        for (int i{}; i < item_count; i++)
        {
            CommonDisplayItem display_item = GetDisplayItem(i);
            std::wstring default_text = display_item.DefaultString(m_main_window_text);
            m_list_ctrl.SetItemText(i, 1, default_text.c_str());
        }
    }
}
