--真魔獣 ガーゼット
function c12338068.initial_effect(c)
	c:EnableReviveLimit()
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(aux.FALSE)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_HAND)
	e2:SetCondition(c12338068.spcon)
	e2:SetOperation(c12338068.spop)
	c:RegisterEffect(e2)
	--pierce
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_PIERCE)
	c:RegisterEffect(e3)
end
function c12338068.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local rg=Duel.GetReleaseGroup(tp)
	return rg:GetCount()>0 and rg:FilterCount(Card.IsControler,nil,tp)==Duel.GetMatchingGroupCount(nil,tp,LOCATION_MZONE,0,nil)
end
function c12338068.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local tp=c:GetControler()
	local g=Duel.GetReleaseGroup(tp)
	local rg=g:Filter(Card.IsControler,nil,tp)
	local rg1=g:Filter(Card.IsControler,nil,1-tp)
	if rg1:GetCount()>0 then
		if rg:GetCount()==0 or Duel.SelectYesNo(tp,aux.Stringid(12338068,0)) then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
			local sg1=rg1:Select(tp,1,rg1:GetCount(),nil)
			rg:Merge(sg1)
		end
	end
	Duel.Release(rg,REASON_COST)
	local atk=rg:GetSum(Card.GetBaseAttack)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_ATTACK)
	e1:SetReset(RESET_EVENT+0xff0000)
	e1:SetValue(atk)
	c:RegisterEffect(e1)
end
