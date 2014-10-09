--禁じられた聖典
function c54773234.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_PRE_DAMAGE_CALCULATE)
	e1:SetCondition(c54773234.condition)
	e1:SetOperation(c54773234.activate)
	c:RegisterEffect(e1)
end
function c54773234.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetAttackTarget()~=nil
end
function c54773234.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_DISABLE)
	e1:SetTargetRange(LOCATION_ONFIELD,LOCATION_ONFIELD)
	e1:SetTarget(c54773234.distg)
	e1:SetReset(RESET_PHASE+PHASE_DAMAGE)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_SOLVING)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c54773234.disop)
	e2:SetReset(RESET_PHASE+PHASE_DAMAGE)
	Duel.RegisterEffect(e2,tp)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_DISABLE_TRAPMONSTER)
	e3:SetRange(LOCATION_MZONE)
	e3:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e3:SetReset(RESET_PHASE+PHASE_DAMAGE)
	Duel.RegisterEffect(e3,tp)
	if a:IsRelateToBattle() then
		local aa=a:GetTextAttack()
		local ad=a:GetTextDefence()
		if a:IsImmuneToEffect(e) then
			aa=a:GetBaseAttack()
			ad=a:GetBaseDefence() end
		if aa<0 then aa=0 end
		if ad<0 then ad=0 end
		local e4=Effect.CreateEffect(c)
		e4:SetType(EFFECT_TYPE_SINGLE)
		e4:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_IGNORE_IMMUNE)
		e4:SetRange(LOCATION_MZONE)
		e4:SetCode(EFFECT_SET_ATTACK_FINAL)
		e4:SetReset(RESET_PHASE+PHASE_DAMAGE)
		e4:SetValue(aa)
		a:RegisterEffect(e4,true)
		local e5=Effect.CreateEffect(c)
		e5:SetType(EFFECT_TYPE_SINGLE)
		e5:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_IGNORE_IMMUNE)
		e5:SetRange(LOCATION_MZONE)
		e5:SetCode(EFFECT_SET_DEFENCE_FINAL)
		e5:SetReset(RESET_PHASE+PHASE_DAMAGE)
		e5:SetValue(ad)
		a:RegisterEffect(e5,true)
	end
	if d and d:IsRelateToBattle() then
		local da=d:GetTextAttack()
		local dd=d:GetTextDefence()
		if d:IsImmuneToEffect(e) then 
			da=d:GetBaseAttack()
			dd=d:GetBaseDefence() end
		if da<0 then da=0 end
		if dd<0 then dd=0 end
		local e6=Effect.CreateEffect(c)
		e6:SetType(EFFECT_TYPE_SINGLE)
		e6:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_IGNORE_IMMUNE)
		e6:SetRange(LOCATION_MZONE)
		e6:SetCode(EFFECT_SET_ATTACK_FINAL)
		e6:SetValue(da)
		e6:SetReset(RESET_PHASE+PHASE_DAMAGE)
		d:RegisterEffect(e6,true)
		local e7=Effect.CreateEffect(c)
		e7:SetType(EFFECT_TYPE_SINGLE)
		e7:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_IGNORE_IMMUNE)
		e7:SetRange(LOCATION_MZONE)
		e7:SetCode(EFFECT_SET_DEFENCE_FINAL)
		e7:SetValue(dd)
		e7:SetReset(RESET_PHASE+PHASE_DAMAGE)
		d:RegisterEffect(e7,true)
	end
end
function c54773234.distg(e,c)
	return c~=e:GetHandler()
end
function c54773234.disop(e,tp,eg,ep,ev,re,r,rp)
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	if bit.band(loc,LOCATION_ONFIELD)~=0 then
		Duel.NegateEffect(ev)
	end
end
