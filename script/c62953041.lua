--稲荷火
function c62953041.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c62953041.hspcon)
	c:RegisterEffect(e1)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetOperation(c62953041.spreg)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(62953041,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_GRAVE)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetCondition(c62953041.spcon)
	e2:SetTarget(c62953041.sptg)
	e2:SetOperation(c62953041.spop)
	c:RegisterEffect(e2)
	--only 1 can exists
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_SUMMON)
	e3:SetCondition(c62953041.excon)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e5:SetCode(EFFECT_SPSUMMON_CONDITION)
	e5:SetValue(c62953041.splimit)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetCode(EFFECT_SELF_DESTROY)
	e6:SetCondition(c62953041.descon)
	c:RegisterEffect(e6)
end
function c62953041.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_SPELLCASTER)
end
function c62953041.hspcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c62953041.filter,c:GetControler(),LOCATION_MZONE,0,1,nil)
		and not Duel.IsExistingMatchingCard(c62953041.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c62953041.spreg(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if bit.band(r,0x41)==0x41 and c:IsPreviousPosition(POS_FACEUP) then
		c:RegisterFlagEffect(62953041,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_STANDBY+RESET_SELF_TURN,0,1)
	end
end
function c62953041.spcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:GetTurnID()~=Duel.GetTurnCount() and tp==Duel.GetTurnPlayer() and c:GetFlagEffect(62953041)>0
end
function c62953041.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local c=e:GetHandler()
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,c,1,0,0)
	c:ResetFlagEffect(62953041)
end
function c62953041.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c62953041.exfilter(c,fid)
	return c:IsFaceup() and c:GetCode()==62953041 and (fid==nil or c:GetFieldID()<fid)
end
function c62953041.excon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c62953041.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c62953041.splimit(e,se,sp,st,spos,tgp)
	if bit.band(spos,POS_FACEDOWN)~=0 then return true end
	return not Duel.IsExistingMatchingCard(c62953041.exfilter,tgp,LOCATION_ONFIELD,0,1,nil)
end
function c62953041.descon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c62953041.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil,c:GetFieldID())
end
