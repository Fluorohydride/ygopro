--フュージョニストキラー
function c98336111.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetCode(EFFECT_SET_ATTACK_FINAL)
	e1:SetCondition(c98336111.condtion)
	e1:SetTarget(c98336111.target)
	e1:SetValue(0)
	c:RegisterEffect(e1)
end
function c98336111.condtion(e)
	local ph=Duel.GetCurrentPhase()
	return (ph==PHASE_DAMAGE or ph==PHASE_DAMAGE_CAL) and Duel.GetAttackTarget()~=nil
		and (Duel.GetAttacker()==e:GetHandler() or Duel.GetAttackTarget()==e:GetHandler())
end
function c98336111.target(e,c)
	return c==e:GetHandler():GetBattleTarget() and c:IsType(TYPE_FUSION)
end
