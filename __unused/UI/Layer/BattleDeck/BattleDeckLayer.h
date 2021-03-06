//
//  BattleDeckLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckLayer_h
#define BattleDeckLayer_h

#include "AbstractUILayer.h"
#include "cocos-ext.h"
#include "CardNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BattleDeckUnitInfoNode;
class FormationData;

class BattleDeckLayerObserver
{
public:
    virtual ~BattleDeckLayerObserver() {}
};

class BattleDeckLayer : public AbstractUILayer, public TableViewDelegate, public TableViewDataSource, public CardNodeObserver
{
public:
    static BattleDeckLayer* create();
    virtual ~BattleDeckLayer();
    void registerObserver(BattleDeckLayerObserver *observer);
    
protected:
    BattleDeckLayer();
    virtual bool init() override;
    
    // LayerColor
    virtual void onEnter() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDelegate
    virtual void tableCellTouched(TableView* table, TableViewCell* cell) override;
    virtual void tableCellHighlight(TableView* table, TableViewCell* cell) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // CardNodeObserver
    virtual void onCardNodeTouchedBegan(CardNode* node) override;
    virtual void onCardNodeTouchedEnded(CardNode* node, bool isValid) override;
    
    Node* createTable(const Size& size);
    ssize_t getCellsCount() const;
    Size getCellSize() const;
    Rect getRealBoundingBox(Node* node) const;
    void reloadTable();
    void configTable(bool reload);
    CardNode* createCardNode(const std::string& name);
    void updateCardNode(CardNode* node, const std::string& name);
    void createDragNode(const std::string& name);
    void removeDragNode();
    void reloadCardDecks();
    void selectCardOnDecks(const std::string& name);
    void cardBackToTable();
    int getIntersectedCardDeckIdx(const Rect& rect) const;
    
    const std::vector<std::string>& getPickedCards() const;
    void loadData();
    void saveData();
    void extract(const std::string& name);
    void insert(const std::string& name);
    void exchange(const std::string& picked, const std::string& candidate);
    
private:
    struct TableViewNode {
        Label* titleLabel;
        Button* topButton;
        Button* bottomButton;
        TableView* table;
        Size maxSize;
        Point topLeftPos;
    };
    
    BattleDeckLayerObserver *_observer;
    Size _cellSize;
    std::string _touchedCard;
    std::string _selectedCard;
    bool _isExtracting;
    
    std::vector<std::string> _candidateCards;
    FormationData* _defaultFormationData;
    
    BattleDeckUnitInfoNode* _infoNode;
    TableViewNode _tableViewNode;
    Label* _selectedCardsLabel;
    std::vector<Sprite*> _cardDecks;
    Node* _dragNode;
};

#endif /* BattleDeckLayer_h */
