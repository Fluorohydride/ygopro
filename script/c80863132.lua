--無効
function c80863132.initial_effect(c)
	--change effect
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c80863132.condition)
	e1:SetOperation(c80863132.activate)
	c:RegisterEffect(e1)
end
function c80863132.condition(e,tp,eg,ep,ev,re,r,rp)
	if not re:IsHasProperty(EFFECT_FLAG_PLAYER_TARGET) then return false end
	if Duel.GetOperationCount(ev)~=1 then return false end
	local ex,cg,cc,cp,cv=Duel.GetOperationInfo(ev,CATEGORY_DRAW)
	return ex and cv>0
end
function c80863132.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeChainOperation(ev,c80863132.repop)
end
function c80863132.repop(e,tp,eg,ep,ev,re,r,rp)
	local ex,cg,cc,cp,cv=Duel.GetOperationInfo(ev,CATEGORY_DRAW)
	if cp<2 then
		Duel.DiscardDeck(cp,cv,REASON_EFFECT)
	end
end
