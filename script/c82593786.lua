--タスケルトン
function c82593786.initial_effect(c)
	--disable attack
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(82593786,0))
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_BATTLE_PHASE)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCountLimit(1,82593786+EFFECT_COUNT_CODE_DUEL)
	e1:SetCondition(c82593786.condition)
	e1:SetCost(c82593786.cost)
	e1:SetOperation(c82593786.operation)
	c:RegisterEffect(e1)
end
function c82593786.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetAttacker()~=nil
end
function c82593786.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c82593786.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateAttack()
end
