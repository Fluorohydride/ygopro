--BF－バックフラッシュ
function c89040386.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c89040386.condition)
	e1:SetTarget(c89040386.target)
	e1:SetOperation(c89040386.activate)
	c:RegisterEffect(e1)
end
function c89040386.cfilter(c)
	return c:IsSetCard(0x33) and c:IsType(TYPE_MONSTER)
end
function c89040386.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer() and Duel.GetAttackTarget()==nil
		and Duel.IsExistingMatchingCard(c89040386.cfilter,tp,LOCATION_GRAVE,0,5,nil)
end
function c89040386.filter(c)
	return c:IsDestructable()
end
function c89040386.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c89040386.filter,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c89040386.filter,tp,0,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c89040386.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c89040386.filter,tp,0,LOCATION_MZONE,nil)
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
