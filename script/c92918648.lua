--魔導法皇 ハイロン
function c92918648.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,c92918648.xyzfilter,2)
	c:EnableReviveLimit()
	--attack up
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(92918648,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c92918648.cost)
	e1:SetTarget(c92918648.target)
	e1:SetOperation(c92918648.operation)
	c:RegisterEffect(e1)
end
function c92918648.xyzfilter(c)
	return c:GetLevel()==7 and c:IsRace(RACE_SPELLCASTER)
end
function c92918648.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c92918648.cfilter(c)
	return c:IsSetCard(0x106e) and c:IsType(TYPE_SPELL)
end
function c92918648.filter(c)
	return c:IsDestructable()
end
function c92918648.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(1-tp) and c92918648.filter(chkc) end
	if chk==0 then return Duel.IsExistingMatchingCard(c92918648.cfilter,tp,LOCATION_GRAVE,0,1,nil)
		and Duel.IsExistingTarget(c92918648.filter,tp,0,LOCATION_ONFIELD,1,nil) end
	local ct=Duel.GetMatchingGroupCount(c92918648.cfilter,tp,LOCATION_GRAVE,0,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c92918648.filter,tp,0,LOCATION_ONFIELD,1,ct,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c92918648.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	Duel.Destroy(g,REASON_EFFECT)
end
