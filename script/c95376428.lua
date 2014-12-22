--召喚制限－エクストラネット
function c95376428.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(95376428,0))
	e2:SetCategory(CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetRange(LOCATION_FZONE)
	e2:SetCondition(c95376428.condition)
	e2:SetTarget(c95376428.target)
	e2:SetOperation(c95376428.operation)
	c:RegisterEffect(e2)
end
function c95376428.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(Card.IsPreviousLocation,1,nil,LOCATION_EXTRA)
end
function c95376428.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) end
end
function c95376428.filter(c,tp)
	return c:GetSummonPlayer()==tp
end
function c95376428.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if eg:IsExists(c95376428.filter,1,nil,tp) and Duel.IsPlayerCanDraw(1-tp,1) and Duel.SelectYesNo(1-tp,aux.Stringid(95376428,1)) then
		Duel.Draw(1-tp,1,REASON_EFFECT)
	end
	if eg:IsExists(c95376428.filter,1,nil,1-tp) and Duel.IsPlayerCanDraw(tp,1) and Duel.SelectYesNo(tp,aux.Stringid(95376428,1)) then
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end
