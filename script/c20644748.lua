--宇宙の収縮
function c20644748.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c20644748.condition)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	c:RegisterEffect(e1)
	--adjust
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e2:SetCode(EVENT_ADJUST)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c20644748.adjustop)
	c:RegisterEffect(e2)
	--
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EFFECT_MAX_MZONE)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetTargetRange(1,1)
	e3:SetValue(c20644748.mvalue)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EFFECT_MAX_SZONE)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetTargetRange(1,1)
	e4:SetValue(c20644748.svalue)
	c:RegisterEffect(e4)
end
function c20644748.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_ONFIELD,0)<=5
		and Duel.GetFieldGroupCount(tp,0,LOCATION_ONFIELD)<=5
end
function c20644748.adjustop(e,tp,eg,ep,ev,re,r,rp)
	local phase=Duel.GetCurrentPhase()
	if (phase==PHASE_DAMAGE and not Duel.IsDamageCalculated()) or phase==PHASE_DAMAGE_CAL then return end
	local c1=Duel.GetFieldGroupCount(tp,LOCATION_ONFIELD,0)
	local c2=Duel.GetFieldGroupCount(tp,0,LOCATION_ONFIELD)
	if c1>5 or c2>5 then
		local g=Group.CreateGroup()
		if c1>5 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
			local g1=Duel.SelectMatchingCard(tp,nil,tp,LOCATION_ONFIELD,0,c1-5,c1-5,nil)
			g:Merge(g1)
		end
		if c2>5 then
			Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
			local g2=Duel.SelectMatchingCard(1-tp,nil,1-tp,LOCATION_ONFIELD,0,c2-5,c2-5,nil)
			g:Merge(g2)
		end
		Duel.SendtoGrave(g,REASON_EFFECT)
		Duel.Readjust()
	end
end
function c20644748.mvalue(e,fp,rp,r)
	return 5-Duel.GetFieldGroupCount(fp,LOCATION_SZONE,0)
end
function c20644748.svalue(e,fp,rp,r)
	local ct=5
	for i=5,7 do
		if Duel.GetFieldCard(fp,LOCATION_SZONE,i) then ct=ct-1 end
	end
	return ct-Duel.GetFieldGroupCount(fp,LOCATION_MZONE,0)
end
