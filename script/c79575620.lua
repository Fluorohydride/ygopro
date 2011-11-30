--お注射天使リリー
function c79575620.initial_effect(c)
	--attack up
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(79575620,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_QUICK_O+EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_PRE_DAMAGE_CALCULATE)
	e1:SetCondition(c79575620.con)
	e1:SetCost(c79575620.cost)
	e1:SetOperation(c79575620.op)
	c:RegisterEffect(e1)
end
function c79575620.con(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:GetFlagEffect(79575620)==0 and (Duel.GetAttacker()==c or Duel.GetAttackTarget()==c)
end
function c79575620.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,2000) end
	Duel.PayLPCost(tp,2000)
	e:GetHandler():RegisterFlagEffect(79575620,RESET_PHASE+RESET_DAMAGE_CAL,0,1)
end
function c79575620.op(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetReset(RESET_PHASE+RESET_DAMAGE_CAL)
	e1:SetValue(3000)
	c:RegisterEffect(e1)
end
