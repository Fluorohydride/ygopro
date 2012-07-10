--グリフォンの羽根帚
function c34370473.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_RECOVER)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c34370473.target)
	e1:SetOperation(c34370473.activate)
	c:RegisterEffect(e1)
end
function c34370473.filter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c34370473.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return Duel.IsExistingMatchingCard(c34370473.filter,tp,LOCATION_ONFIELD,0,1,c) end
	local g=Duel.GetMatchingGroup(c34370473.filter,tp,LOCATION_ONFIELD,0,c)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,g:GetCount()*500)
end
function c34370473.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c34370473.filter,tp,LOCATION_ONFIELD,0,e:GetHandler())
	local ct=Duel.Destroy(g,REASON_EFFECT)
	Duel.Recover(tp,ct*500,REASON_EFFECT)
end
