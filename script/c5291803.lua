--先史遺産トゥーラ・ガーディアン
function c5291803.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c5291803.spcon)
	c:RegisterEffect(e1)
	--count limit
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_SUMMON)
	e3:SetCondition(c5291803.excon)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e5:SetCode(EFFECT_SPSUMMON_CONDITION)
	e5:SetValue(c5291803.splimit)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetCode(EFFECT_SELF_DESTROY)
	e6:SetCondition(c5291803.descon)
	c:RegisterEffect(e6)
end
function c5291803.cfilter(tc)
	return tc and tc:IsFaceup()
end
function c5291803.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and (c5291803.cfilter(Duel.GetFieldCard(tp,LOCATION_SZONE,5)) or c5291803.cfilter(Duel.GetFieldCard(1-tp,LOCATION_SZONE,5)))
		and not Duel.IsExistingMatchingCard(c5291803.exfilter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c5291803.exfilter(c,fid)
	return c:IsFaceup() and c:GetCode()==5291803 and (fid==nil or c:GetFieldID()<fid)
end
function c5291803.excon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c5291803.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c5291803.splimit(e,se,sp,st,spos,tgp)
	if bit.band(spos,POS_FACEDOWN)~=0 then return true end
	return not Duel.IsExistingMatchingCard(c5291803.exfilter,tgp,LOCATION_ONFIELD,0,1,nil)
end
function c5291803.descon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c5291803.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil,c:GetFieldID())
end
