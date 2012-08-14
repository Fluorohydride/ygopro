--岩盤爆破
function c85519211.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c85519211.target)
	e1:SetOperation(c85519211.activate)
	c:RegisterEffect(e1)
end
function c85519211.filter(c)
	return c:IsFaceup() and c:IsCode(76321376)
end
function c85519211.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c85519211.filter,tp,LOCATION_MZONE,0,1,nil) end
	local g=Duel.GetMatchingGroup(c85519211.filter,tp,LOCATION_MZONE,0,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,g:GetCount()*1000)
end
function c85519211.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c85519211.filter,tp,LOCATION_MZONE,0,nil)
	Duel.Damage(1-tp,g:GetCount()*1000,REASON_EFFECT)
	Duel.Destroy(g,REASON_EFFECT)
end
