--運命の火時計
function c1082946.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c1082946.target)
	e1:SetOperation(c1082946.activate)
	c:RegisterEffect(e1)
end
function c1082946.filter(c)
	return c:GetFlagEffect(1082946)~=0
end
function c1082946.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c1082946.filter,tp,0x3f,0x3f,1,nil) end
end
function c1082946.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(1082946,0))
	local g=Duel.SelectMatchingCard(tp,c1082946.filter,tp,0x3f,0x3f,1,1,nil)
	local tc=g:GetFirst()
	local turne=tc[tc]
	local op=turne:GetOperation()
	op(turne,turne:GetOwnerPlayer(),nil,0,0,0,0,0)
end
