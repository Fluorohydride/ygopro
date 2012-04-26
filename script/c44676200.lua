--ヒーローバリア
function c44676200.initial_effect(c)
	--negate attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c44676200.condition)
	e1:SetOperation(c44676200.operation)
	c:RegisterEffect(e1)
end
function c44676200.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x3008)
end
function c44676200.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and Duel.GetCurrentPhase()<PHASE_MAIN2
		and Duel.IsExistingMatchingCard(c44676200.filter,tp,LOCATION_MZONE,0,1,nil)
end
function c44676200.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetAttacker() then Duel.NegateAttack()
	else
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_ATTACK_ANNOUNCE)
		e1:SetReset(RESET_PHASE+PHASE_END)
		e1:SetOperation(c44676200.disop)
		Duel.RegisterEffect(e1,tp)
	end
end
function c44676200.disop(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateAttack()
	e:Reset()
end
