--セイクリッドの流星
function c63545861.initial_effect(c)
	--
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetOperation(c63545861.regop)
	c:RegisterEffect(e1)
end
function c63545861.regop(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetOperation(c63545861.retop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c63545861.retop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	local g=Group.CreateGroup()
	if a:IsSetCard(0x53) and d and d:IsControler(1-tp) and d:IsRelateToBattle() then g:AddCard(d) end
	if d and d:IsSetCard(0x53) and a:IsControler(1-tp) and a:IsRelateToBattle() then g:AddCard(a) end
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
end
