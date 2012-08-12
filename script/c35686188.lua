--断頭台の惨劇(EU)
function c35686188.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHANGE_POS)
	e1:SetCondition(c35686188.condition)
	e1:SetTarget(c35686188.target)
	e1:SetOperation(c35686188.activate)
	c:RegisterEffect(e1)
end
function c35686188.cfilter(c,tp)
	return c:IsControler(tp) and c:IsPreviousPosition(POS_FACEUP_ATTACK) and c:IsPosition(POS_FACEUP_DEFENCE)
end
function c35686188.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c35686188.cfilter,1,nil,1-tp)
end
function c35686188.filter(c)
	return c:IsDefencePos() and c:IsDestructable()
end
function c35686188.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c35686188.filter,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c35686188.filter,tp,0,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c35686188.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c35686188.filter,tp,0,LOCATION_MZONE,nil)
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
