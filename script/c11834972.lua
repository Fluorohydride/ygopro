--ラヴァル・ガンナー
function c11834972.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(11834972,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCondition(c11834972.condition)
	e1:SetCost(c11834972.cost)
	e1:SetOperation(c11834972.operation)
	c:RegisterEffect(e1)
end
function c11834972.cfilter(c)
	return c:IsSetCard(0x39) and c:GetCode()~=11834972
end
function c11834972.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c11834972.cfilter,tp,LOCATION_GRAVE,0,1,nil)
end
function c11834972.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDiscardDeckAsCost(tp,1) end
	local ct=Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)
	if ct>5 then ct=5 end
	local t={}
	for i=1,ct do t[i]=i end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(11834972,1))
	local ac=Duel.AnnounceNumber(tp,table.unpack(t))
	Duel.DiscardDeck(tp,ac,REASON_COST)
	local g=Duel.GetOperatedGroup()
	e:SetLabel(g:FilterCount(Card.IsSetCard,nil,0x39)*200)
end
function c11834972.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		e1:SetValue(e:GetLabel())
		c:RegisterEffect(e1)
	end
end
