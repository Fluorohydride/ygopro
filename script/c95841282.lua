--ネコ耳族
function c95841282.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SET_BASE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetTarget(c95841282.atktg)
	e1:SetCondition(c95841282.atkcon)
	e1:SetValue(200)
	c:RegisterEffect(e1)
end
function c95841282.atkcon(e)
	local ph=Duel.GetCurrentPhase()
	return (ph==PHASE_DAMAGE or ph==PHASE_DAMAGE_CAL) and Duel.GetTurnPlayer()~=e:GetHandlerPlayer()
		and Duel.GetAttackTarget()==e:GetHandler()
end
function c95841282.atktg(e,c)
	return c==e:GetHandler():GetBattleTarget()
end
