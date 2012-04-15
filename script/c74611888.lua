--手のひら返し
function c74611888.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCondition(c74611888.condition)
	e1:SetTarget(c74611888.target)
	e1:SetOperation(c74611888.activate)
	c:RegisterEffect(e1)
end
function c74611888.cfilter(c)
	return c:IsFaceup() and c:GetLevel()~=c:GetOriginalLevel()
end
function c74611888.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c74611888.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil)
end
function c74611888.filter(c)
	return c:IsFaceup() and c:IsCanTurnSet()
end
function c74611888.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c74611888.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c74611888.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
end
function c74611888.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c74611888.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if g:GetCount()>0 then
		Duel.ChangePosition(g,POS_FACEDOWN_DEFENCE)
	end
end
