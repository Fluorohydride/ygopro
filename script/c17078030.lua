--光の護封壁
function c17078030.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c17078030.cost)
	c:RegisterEffect(e1)
	--cannot attack
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_ATTACK)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(0,LOCATION_MZONE)
	e2:SetTarget(c17078030.atktarget)
	c:RegisterEffect(e2)
	e1:SetLabelObject(e2)
end
function c17078030.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	local lp=Duel.GetLP(tp)
	local t={}
	local f=math.floor((lp)/1000)
	local l=1
	while l<=f and l<=20 do
		t[l]=l*1000
		l=l+1
	end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(17078030,0))
	local announce=Duel.AnnounceNumber(tp,table.unpack(t))
	Duel.PayLPCost(tp,announce)
	e:GetLabelObject():SetLabel(announce)
	e:GetHandler():SetHint(CHINT_NUMBER,announce)
end
function c17078030.atktarget(e,c)
	return c:GetAttack()<=e:GetLabel()
end
