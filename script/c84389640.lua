--窮鼠の進撃
function c84389640.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--atkup
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetDescription(aux.Stringid(84389640,0))
	e2:SetCategory(CATEGORY_ATKCHANGE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(TIMING_DAMAGE_CAL)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e2:SetCondition(c84389640.condition)
	e2:SetCost(c84389640.cost)
	e2:SetOperation(c84389640.operation)
	c:RegisterEffect(e2)
end
function c84389640.condition(e,tp,eg,ep,ev,re,r,rp)
	local phase=Duel.GetCurrentPhase()
	if (phase~=PHASE_DAMAGE and phase~=PHASE_DAMAGE_CAL) or Duel.IsDamageCalculated() then return false end
	local tc=Duel.GetAttacker()
	local bc=Duel.GetAttackTarget()
	if tc:IsControler(1-tp) then
		tc=Duel.GetAttackTarget()
		bc=Duel.GetAttacker()
	end
	e:SetLabelObject(bc)
	return tc and bc and tc:IsFaceup() and bc:IsFaceup() and tc:IsLevelBelow(3) and tc:IsType(TYPE_NORMAL) and tc:IsRelateToBattle()
end
function c84389640.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(84389640)==0 and Duel.CheckLPCost(tp,100)
		and e:GetLabelObject():IsAttackAbove(100) end
	local bc=e:GetLabelObject()
	local lp=Duel.GetLP(tp)
	local atk=e:GetLabelObject():GetAttack()
	local maxc=lp>atk and atk or lp
	maxc=math.floor(maxc/100)*100
	local t={}
	for i=1,maxc/100 do
		t[i]=i*100
	end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(84389640,1))
	local pay=Duel.AnnounceNumber(tp,table.unpack(t))
	Duel.PayLPCost(tp,pay)
	e:SetLabel(-pay)
	e:GetHandler():RegisterFlagEffect(84389640,RESET_PHASE+RESET_DAMAGE_CAL,0,1)
end
function c84389640.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local bc=e:GetLabelObject()
	if bc:IsRelateToBattle() and bc:IsFaceup() then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetOwnerPlayer(tp)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e1:SetValue(e:GetLabel())
		bc:RegisterEffect(e1)
	end
end
