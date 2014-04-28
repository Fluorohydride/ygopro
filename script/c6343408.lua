--奇跡の発掘
function c6343408.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c6343408.condition)
	e1:SetTarget(c6343408.target)
	e1:SetOperation(c6343408.activate)
	c:RegisterEffect(e1)
end
function c6343408.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_MONSTER)
end
function c6343408.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c6343408.filter,tp,LOCATION_REMOVED,0,5,nil)
end
function c6343408.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_REMOVED) and chkc:IsControler(tp) and c6343408.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c6343408.filter,tp,LOCATION_REMOVED,0,3,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(6343408,0))
	local g=Duel.SelectTarget(tp,c6343408.filter,tp,LOCATION_REMOVED,0,3,3,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,g,3,0,0)
end
function c6343408.activate(e,tp,eg,ep,ev,re,r,rp)
	local tg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=tg:Filter(Card.IsRelateToEffect,nil,e)
	if sg:GetCount()>0 then
		Duel.SendtoGrave(sg,REASON_EFFECT+REASON_RETURN)
	end
end
