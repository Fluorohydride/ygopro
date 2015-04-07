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
	return c:IsFaceup() and c:IsSetCard(0xae) and c:GetSequence()<5 and c:IsDestructable()
end
function c46259438.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp)
		and Duel.IsExistingMatchingCard(c46259438.filter,tp,LOCATION_SZONE,0,1,nil) end
	local g=Duel.GetMatchingGroup(c46259438.filter,tp,LOCATION_SZONE,0,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,g:GetCount())
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,g:GetCount()*1000)
end
function c46259438.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c46259438.filter,tp,LOCATION_SZONE,0,nil)
	local ct1=Duel.Destroy(g,REASON_EFFECT)
	if ct1==0 then return end
	local ct2=Duel.Draw(tp,ct1,REASON_EFFECT)
	Duel.BreakEffect()
	Duel.Recover(tp,ct2*1000,REASON_EFFECT)
end
