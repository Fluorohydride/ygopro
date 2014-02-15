--ラスト・カウンター
function c86049351.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c86049351.condition)
	e1:SetTarget(c86049351.target)
	e1:SetOperation(c86049351.activate)
	c:RegisterEffect(e1)
end
function c86049351.condition(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetAttacker()
	local at=Duel.GetAttackTarget()
	if not tc:IsControler(tp) then tc=at end
	e:SetLabelObject(tc)
	return at and tc:IsControler(tp) and tc:IsLocation(LOCATION_MZONE) and tc:IsSetCard(0x84)
end
function c86049351.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x84)
end
function c86049351.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c86049351.filter,tp,LOCATION_MZONE,0,1,e:GetLabelObject()) end
end
function c86049351.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	local bc=tc:GetBattleTarget()
	if Duel.NegateAttack() and tc:IsRelateToBattle() and bc:IsRelateToBattle() then
		Duel.SendtoGrave(tc,REASON_EFFECT)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
		local g=Duel.SelectMatchingCard(tp,c86049351.filter,tp,LOCATION_MZONE,0,1,1,nil)
		local sc=g:GetFirst()
		local atk=bc:GetBaseAttack()
		if atk<0 then atk=0 end
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(atk)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		sc:RegisterEffect(e1)
		Duel.CalculateDamage(bc,sc)
		Duel.BreakEffect()
		Duel.Damage(tp,atk,REASON_EFFECT)
	end
end
