--連鎖破壊
function c1248895.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c1248895.target)
	e1:SetOperation(c1248895.activate)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e3)
end
function c1248895.filter(c,e)
	return c:IsFaceup() and c:IsAttackBelow(2000) and c:IsCanBeEffectTarget(e)
end
function c1248895.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return eg:IsContains(chkc) end
	if chk==0 then return eg:IsExists(c1248895.filter,1,nil,e) end
	if eg:GetCount()==1 then
		Duel.SetTargetCard(eg)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
		local g=eg:FilterSelect(tp,c1248895.filter,1,1,nil,e)
		Duel.SetTargetCard(g)
	end
end
function c1248895.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local tpe=tc:GetType()
	if bit.band(tpe,TYPE_TOKEN)~=0 then return end
	local dg=Duel.GetMatchingGroup(Card.IsCode,tc:GetControler(),LOCATION_DECK+LOCATION_HAND,0,nil,tc:GetCode())
	Duel.Destroy(dg,REASON_EFFECT)
end
