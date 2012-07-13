--「A」細胞培養装置
function c64163367.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--counter1
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_REMOVE_COUNTER+0xe)
	e2:SetOperation(c64163367.ctop1)
	c:RegisterEffect(e2)
	--counter2
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(64163367,0))
	e2:SetCategory(CATEGORY_COUNTER)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetCondition(c64163367.ctcon2)
	e2:SetOperation(c64163367.ctop2)
	c:RegisterEffect(e2)
end
function c64163367.ctop1(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():AddCounter(0xe,1)
end
function c64163367.ctcon2(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetHandler():GetCounter(0xe)
	e:SetLabel(ct)
	return e:GetHandler():IsReason(REASON_DESTROY) and ct>0
end
function c64163367.ctop2(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if g:GetCount()==0 then return end
	for i=1,ct do
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(64163367,1))
		local sg=g:Select(tp,1,1,nil)
		sg:GetFirst():AddCounter(0xe,1)
	end
end
