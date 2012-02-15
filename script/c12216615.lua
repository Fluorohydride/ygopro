--コアキメイルの障壁
function c12216615.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c12216615.condition)
	e1:SetTarget(c12216615.target)
	e1:SetOperation(c12216615.activate)
	c:RegisterEffect(e1)
end
function c12216615.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer() and Duel.IsExistingMatchingCard(Card.IsCode,tp,LOCATION_GRAVE,0,2,nil,36623431)
end
function c12216615.filter(c)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsDestructable()
end
function c12216615.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c12216615.filter,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c12216615.filter,tp,0,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c12216615.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c12216615.filter,tp,0,LOCATION_MZONE,nil)
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
