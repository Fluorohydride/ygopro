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
	if not at or tc:IsFacedown() or at:IsFacedown() then return false end
	if not tc:IsControler(tp) then tc=at end
	e:SetLabelObject(tc)
	return tc:IsControler(tp) and tc:IsLocation(LOCATION_MZONE) and tc:IsSetCard(0x84)
end
function c86049351.filter(c,pos)
	return c:IsPosition(pos) and c:IsSetCard(0x84)
end
function c86049351.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local tc=e:GetLabelObject()
		local bc=tc:GetBattleTarget()
		local pos=POS_FACEUP
		if bc==Duel.GetAttackTarget() then pos=POS_FACEUP_ATTACK end
		return Duel.IsExistingMatchingCard(c86049351.filter,tp,LOCATION_MZONE,0,1,tc,pos)
	end
end
function c86049351.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	local bc=tc:GetBattleTarget()
	if Duel.NegateAttack() and tc:IsRelateToBattle() and bc:IsRelateToBattle() and bc:IsFaceup() then
		Duel.SendtoGrave(tc,REASON_EFFECT)
		local pos=POS_FACEUP
		if bc==Duel.GetAttackTarget() then pos=POS_FACEUP_ATTACK end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
		local g=Duel.SelectMatchingCard(tp,c86049351.filter,tp,LOCATION_MZONE,0,1,1,nil,pos)
		local sc=g:GetFirst()
		local atk=bc:GetBaseAttack()
		if atk<0 then atk=0 end
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(atk)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		sc:RegisterEffect(e1)
		if bc==Duel.GetAttackTarget() then bc,sc=sc,bc end
		if bc:IsAttackable() and not bc:IsImmuneToEffect(e) and not sc:IsImmuneToEffect(e) then
			Duel.CalculateDamage(bc,sc)
			Duel.BreakEffect()
			Duel.Damage(tp,atk,REASON_EFFECT)
		end
	end
end
