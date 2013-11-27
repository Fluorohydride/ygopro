--サイコ·コマンダー
function c21454943.initial_effect(c)
	--atkdown
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetDescription(aux.Stringid(21454943,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DAMAGE_CAL)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c21454943.condition)
	e1:SetCost(c21454943.cost)
	e1:SetTarget(c21454943.target)
	e1:SetOperation(c21454943.operation)
	c:RegisterEffect(e1)
end
function c21454943.condition(e,tp,eg,ep,ev,re,r,rp)
	local phase=Duel.GetCurrentPhase()
	if (phase~=PHASE_DAMAGE and phase~=PHASE_DAMAGE_CAL) or Duel.IsDamageCalculated() then return false end
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return (d~=nil and a:GetControler()==tp and a:IsRace(RACE_PSYCHO) and a:IsRelateToBattle())
		or (d~=nil and d:GetControler()==tp and d:IsRace(RACE_PSYCHO) and d:IsRelateToBattle())
end
function c21454943.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,100) and e:GetHandler():GetFlagEffect(21454943)==0 end
	local lp=Duel.GetLP(tp)
	local alp=100
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(21454943,1))
	if lp>500 then alp=Duel.AnnounceNumber(tp,100,200,300,400,500)
	elseif lp>400 then alp=Duel.AnnounceNumber(tp,100,200,300,400)
	elseif lp>300 then alp=Duel.AnnounceNumber(tp,100,200,300)
	elseif lp>200 then alp=Duel.AnnounceNumber(tp,100,200)
	end
	Duel.PayLPCost(tp,alp)
	e:SetLabel(-alp)
	e:GetHandler():RegisterFlagEffect(21454943,RESET_PHASE+RESET_DAMAGE,0,1)
end
function c21454943.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if chkc then return (a:GetControler()==tp and chkc==d) or (d:GetControler()==tp and chkc==a) end
	if chk==0 then
		if a:GetControler()==tp then
			return a:IsRace(RACE_PSYCHO) and d and d:IsCanBeEffectTarget(e)
		else return d:IsRace(RACE_PSYCHO) and a:IsCanBeEffectTarget(e) end
	end
	if a:GetControler()==tp then Duel.SetTargetCard(d)
	else Duel.SetTargetCard(a) end
end
function c21454943.operation(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=Duel.GetFirstTarget()
	local c=e:GetHandler()
	if not tc or not tc:IsRelateToEffect(e) or not c:IsRelateToEffect(e) then return end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	e1:SetValue(e:GetLabel())
	tc:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	tc:RegisterEffect(e2)
end
