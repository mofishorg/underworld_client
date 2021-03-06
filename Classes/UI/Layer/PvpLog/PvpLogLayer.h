//
//  PvpLogLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef PvpLogLayer_h
#define PvpLogLayer_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "PvpLogNode.h"
#include "PvpLogShareLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;

class PvpLogLayerObserver
{
public:
    virtual ~PvpLogLayerObserver() {}
    virtual void onPvpLogLayerClickedShareButton(Node* pSender) = 0;
};

class PvpLogLayer
: public LayerColor
, public TableViewDataSource
, public PvpLogNodeObserver
, public PvpLogShareLayerObserver
{
public:
    static PvpLogLayer* create();
    virtual ~PvpLogLayer();
    void registerObserver(PvpLogLayerObserver *observer);
    
protected:
    PvpLogLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // PvpLogNodeObserver
    virtual void onPvpLogNodeReplay(const PvpLogData* data) override;
    virtual void onPvpLogNodeShare(const PvpLogData* data) override;
    virtual void onPvpLogNodeExpand(Node* sender, bool expanded) override;
    
    // PvpLogShareLayerObserver
    virtual void onPvpLogShareLayerClickedExitButton(Node* pSender) override;
    virtual void onPvpLogShareLayerClickedShareButton(Node* pSender, const std::string& msg) override;
    
    // table
    void createTable(Node* parent);
    void refreshTable(bool reload);
    ssize_t getCellsCount() const;
    float getNodeHeight(ssize_t idx) const;
    const std::vector<PvpLogData*>& getData() const;
    
private:
    PvpLogLayerObserver *_observer;
    TableView* _table;
    ssize_t _expandedIdx;
};

#endif /* PvpLogLayer_h */
