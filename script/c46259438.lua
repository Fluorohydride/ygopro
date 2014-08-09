--契約洗浄
function c46259438.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DRAW+CATEGORY_RECOVER)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c46259438.target)
	e1:SetOperation(c46259438.activate)
	c:RegisterEffect(e1)
end
function c46259438.filter(c)
	return c:IsSetCard(0xae) and c:IsDestructable()
end
function c46259438.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1)
		and  Duel.IsExistingMatchingCard(c46259438.filter,tp,LOCATION_SZONE,0,1,c) 	
	end
	local g=Duel.GetMatchingGroup(c46259438.filter,tp,LOCATION_SZONE,0,c)
	local ct=g:GetCount()
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,ct,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,ct*1000)
end
function c46259438.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c46259438.filter,tp,LOCATION_ONFIELD,0,e:GetHandler())
	local ct=Duel.Destroy(g,REASON_EFFECT)
	if ct==0 then return end
	if Duel.Draw(tp,ct,REASON_EFFECT)>0 then
		Duel.BreakEffect()
		Duel.Recover(tp,ct*1000,REASON_EFFECT)
	end
end
