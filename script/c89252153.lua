--E·HERO ネクロダークマン
function c89252153.initial_effect(c)
	--summon with no tribute
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(89252153,0))
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetTargetRange(LOCATION_HAND,0)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c89252153.ntcon)
	e1:SetTarget(c89252153.nttg)
	e1:SetOperation(c89252153.ntop)
	c:RegisterEffect(e1)
end
function c89252153.ntcon(e,c)
	if e:GetHandler():GetFlagEffect(89252153)~=0 then return false end
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
end
function c89252153.nttg(e,c)
	return c:IsLevelAbove(5) and c:IsSetCard(0x3008)
end
function c89252153.ntop(e,tp,eg,ep,ev,re,r,rp,c)
	e:GetHandler():RegisterFlagEffect(89252153,RESET_EVENT+0x1fe0000,0,1)
end
