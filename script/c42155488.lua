--ジェノミックス・ファイター
function c42155488.initial_effect(c)
	--summon with no tribute
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(42155488,0))
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c42155488.ntcon)
	e1:SetOperation(c42155488.ntop)
	c:RegisterEffect(e1)
	--declear
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(42155488,1))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetTarget(c42155488.dectg)
	e2:SetOperation(c42155488.decop)
	c:RegisterEffect(e2)
	if not c42155488.globalcheck then
		c42155488.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge1:SetOperation(c42155488.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=ge1:Clone()
		ge2:SetCode(EVENT_SUMMON_SUCCESS)
		Duel.RegisterEffect(ge2,0)
	end
end
function c42155488.ntcon(e,c)
	if c==nil then return true end
	return c:GetLevel()>4 and Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
end
function c42155488.ntop(e,tp,eg,ep,ev,re,r,rp,c)
	--change base attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetReset(RESET_EVENT+0xff0000)
	e1:SetCode(EFFECT_SET_BASE_ATTACK)
	e1:SetValue(1100)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CHANGE_LEVEL)
	e2:SetValue(3)
	c:RegisterEffect(e2)
end
function c42155488.dectg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,42155488)==0 
	   or c42155488[tp]~=0
	end
	Duel.Hint(HINT_SELECTMSG,tp,563)
	if Duel.GetFlagEffect(tp,42155488)==0 then
		local rc=Duel.AnnounceRace(tp,1,0xffffff)
	else 
		local rc=Duel.AnnounceRace(tp,1,c42155488[tp])
	end
	e:SetLabel(rc)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c42155488.sumlimit)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetLabel(rc)
	Duel.RegisterEffect(e1,tp)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	Duel.RegisterEffect(e2,tp)
end
function c42155488.sumlimit(e,c)
	return not c:IsRace(e:GetLabel())
end
function c42155488.decop(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local rc=e:GetLabel()
		c:SetHint(CHINT_RACE,rc)
		local e1=Effect.CreateEffect(c)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SYNCHRO_CHECK)
		e1:SetValue(c42155488.syncheck)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e1:SetLabel(rc)
		c:RegisterEffect(e1)
	end
end
function c42155488.syncheck(e,c)
	c:AssumeProperty(ASSUME_RACE,e:GetLabel())
end

function c42155488.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		local sump=tc:GetSummonPlayer()
		local race=tc:GetRace()
		if Duel.GetFlagEffect(sump,42155488)==0 then
		    c42155488[sump]=race
			Duel.RegisterFlagEffect(sump,42155488,RESET_PHASE+PHASE_END,0,1)
	    else if c42155488[sump]~= race then
			c42155488[sump]=0
			end
		end
		tc=eg:GetNext()
	end
end
