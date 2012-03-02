--爆導索
function c99788587.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCondition(c99788587.condition)
	e1:SetTarget(c99788587.target)
	e1:SetOperation(c99788587.activate)
	c:RegisterEffect(e1)
end
function c99788587.condition(e,tp,eg,ep,ev,re,r,rp)
	local seq=e:GetHandler():GetSequence()
	return e:GetHandler():IsFacedown()
		and Duel.GetFieldCard(tp,LOCATION_MZONE,seq)
		and Duel.GetFieldCard(tp,LOCATION_SZONE,seq)
		and Duel.GetFieldCard(1-tp,LOCATION_MZONE,4-seq)
		and Duel.GetFieldCard(1-tp,LOCATION_SZONE,4-seq)
end
function c99788587.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chk==0 then return true end
	local seq=e:GetHandler():GetSequence()
	local g=Group.CreateGroup()
	g:AddCard(Duel.GetFieldCard(tp,LOCATION_MZONE,seq))
	g:AddCard(Duel.GetFieldCard(tp,LOCATION_SZONE,seq))
	g:AddCard(Duel.GetFieldCard(1-tp,LOCATION_MZONE,4-seq))
	g:AddCard(Duel.GetFieldCard(1-tp,LOCATION_SZONE,4-seq))
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c99788587.activate(e,tp,eg,ep,ev,re,r,rp)
	local seq=e:GetHandler():GetSequence()
	local g=Group.CreateGroup()
	local tc=Duel.GetFieldCard(tp,LOCATION_MZONE,seq)
	if tc then g:AddCard(tc) end
	tc=Duel.GetFieldCard(tp,LOCATION_SZONE,seq)
	if tc then g:AddCard(tc) end
	tc=Duel.GetFieldCard(1-tp,LOCATION_MZONE,4-seq)
	if tc then g:AddCard(tc) end
	tc=Duel.GetFieldCard(1-tp,LOCATION_SZONE,4-seq)
	if tc then g:AddCard(tc) end
	Duel.Destroy(g,REASON_EFFECT)
end
