--機雷化
function c89086566.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0)
	e1:SetTarget(c89086566.target)
	e1:SetOperation(c89086566.activate)
	c:RegisterEffect(e1)
end
function c89086566.cfilter(c)
	local code=c:GetCode()
	return c:IsFaceup() and (code==40640057 or code==40703223) and c:IsDestructable()
end
function c89086566.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c89086566.cfilter,tp,LOCATION_MZONE,0,1,nil) end
	local g=Duel.GetMatchingGroup(c89086566.cfilter,tp,LOCATION_MZONE,0,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c89086566.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c89086566.cfilter,tp,LOCATION_MZONE,0,nil)
	local dt=Duel.Destroy(g,REASON_EFFECT)
	if dt==0 then return end
	local dg=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	if dg:GetCount()>0 then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local sg=dg:Select(tp,1,dt,nil)
		Duel.Destroy(sg,REASON_EFFECT)
	end
end
