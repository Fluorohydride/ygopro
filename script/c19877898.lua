--アルティメット・インセクト LV7
function c19877898.initial_effect(c)
	--atk,def down
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetCondition(c19877898.con)
	e1:SetValue(-700)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e2)
end
c19877898.lvupcount=1
c19877898.lvup={34830502}
c19877898.lvdncount=3
c19877898.lvdn={49441499,34088136,34830502}
function c19877898.con(e)
	return e:GetHandler():GetFlagEffect(19877898)~=0
end
