--冥王の咆哮
function c41925941.initial_effect(c)
	--atkdown
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DAMAGE_CAL)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e1:SetCondition(c41925941.condition)
	e1:SetCost(c41925941.cost)
	e1:SetOperation(c41925941.operation)
	c:RegisterEffect(e1)
end
function c41925941.condition(e,tp,eg,ep,ev,re,r,rp)
	local phase=Duel.GetCurrentPhase()
	if (phase~=PHASE_DAMAGE and phase~=PHASE_DAMAGE_CAL) or Duel.IsDamageCalculated() then return false end
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if a:IsControler(tp) then 
		e:SetLabelObject(d)
		return d and a:IsRace(RACE_FIEND) and a:IsRelateToBattle() and d:IsFaceup() and d:IsRelateToBattle()
	else
		e:SetLabelObject(a)
		return a and a:IsFaceup() and a:IsRace(RACE_FIEND) and a:IsRelateToBattle()
	end
end
function c41925941.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local bc=e:GetLabelObject()
	if chk==0 then return Duel.GetLP(tp)>100 and bc:IsAttackAbove(100) and bc:IsDefenceAbove(100) end
	local maxc=Duel.GetLP(tp)
	local atk=bc:GetAttack()
	local def=bc:GetDefence()
	if atk<maxc then maxc=atk end
	if def<maxc then maxc=def end
	if maxc>5000 then maxc=5000 end
	local t={}
	for i=1,maxc/100 do
		t[i]=i*100
	end
	local cost=Duel.AnnounceNumber(tp,table.unpack(t))
	Duel.PayLPCost(tp,cost)
	e:SetLabel(cost)
end
function c41925941.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	local bc=e:GetLabelObject()
	local val=e:GetLabel()
	if not bc:IsRelateToBattle() then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(-val)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	bc:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	bc:RegisterEffect(e2)
end
