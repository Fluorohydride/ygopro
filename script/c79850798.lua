--爆走特急ロケット・アロー
function c79850798.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c79850798.spcon)
	e1:SetOperation(c79850798.spop)
	c:RegisterEffect(e1)
	--cannot special summon
	local e2=Effect.CreateEffect(c)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e2)
	--
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_ACTIVATE)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetRange(LOCATION_MZONE)
	e3:SetTargetRange(1,0)
	e3:SetValue(1)
	c:RegisterEffect(e3)
	--cannot set
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_CANNOT_MSET)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetRange(LOCATION_MZONE)
	e4:SetTargetRange(1,0)
	e4:SetTarget(aux.TRUE)
	c:RegisterEffect(e4)
	local e5=e4:Clone()
	e5:SetCode(EFFECT_CANNOT_SSET)
	c:RegisterEffect(e5)
	local e6=e4:Clone()
	e6:SetCode(EFFECT_CANNOT_TURN_SET)
	c:RegisterEffect(e6)
	local e7=e4:Clone()
	e7:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e7:SetTarget(c79850798.sumlimit)
	c:RegisterEffect(e7)
	--maintain
	local e8=Effect.CreateEffect(c)
	e8:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e8:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e8:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e8:SetRange(LOCATION_MZONE)
	e8:SetCountLimit(1)
	e8:SetOperation(c79850798.mtop)
	c:RegisterEffect(e8)
end
function c79850798.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetActivityCount(tp,ACTIVITY_BATTLE_PHASE)==0
		and Duel.GetFieldGroupCount(tp,LOCATION_ONFIELD,0)==0
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
end
function c79850798.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_BP)
	e1:SetTargetRange(1,0)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c79850798.sumlimit(e,c,sump,sumtype,sumpos,targetp)
	return bit.band(sumpos,POS_FACEDOWN)~=0
end
function c79850798.mtop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()~=tp then return end
	if Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)~=0 and Duel.SelectYesNo(tp,aux.Stringid(79850798,1)) then
		Duel.SendtoGrave(Duel.GetFieldGroup(tp,LOCATION_HAND,0),REASON_COST)
	else
		Duel.Destroy(e:GetHandler(),REASON_RULE)
	end
end
