--サイバー・オーガ
function c64268668.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(64268668,0))
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetHintTiming(TIMING_BATTLE_PHASE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c64268668.atkcon)
	e1:SetCost(c64268668.atkcost)
	e1:SetTarget(c64268668.atktg)
	e1:SetOperation(c64268668.atkop)
	c:RegisterEffect(e1)
end
function c64268668.atkcon(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return Duel.GetCurrentPhase()==PHASE_BATTLE
		and ((a and a:IsControler(tp) and a:IsFaceup() and a:IsCode(64268668))
		or (d and d:IsControler(tp) and d:IsFaceup() and d:IsCode(64268668)))
end
function c64268668.atkcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST+REASON_DISCARD)
end
function c64268668.atktg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if chk==0 then
		if a:IsControler(tp) then return a:IsCanBeEffectTarget(e)
		else return d:IsCanBeEffectTarget(e) end
	end
	if a:IsControler(tp) then return Duel.SetTargetCard(a)
	else return Duel.SetTargetCard(d) end
end
function c64268668.atkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.NegateAttack()
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(2000)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_DAMAGE_STEP_END)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		e2:SetOperation(c64268668.resetop)
		e2:SetLabelObject(e1)
		tc:RegisterEffect(e2)
	end
end
function c64268668.resetop(e,tp,eg,ep,ev,re,r,rp)
	e:GetLabelObject():Reset()
	e:Reset()
end
